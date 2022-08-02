#include "mgl.h"
#include "model.h"
#include <cmath>
#include <cstdio>

SDL_Window* window;
SDL_Renderer* renderer;
int width;
int height;
static int* pixels;
static int pitch;

vec4 init_col;
SDL_Texture* buffer;
float init_z;
std::vector<float> zbuffer;

struct Tr_element {
    vec4 points[3];
    floatstream varyings[3];
};

struct Component {
    Component(int size, float * data) : size(size), data(data) {}
    int size; // element size (num of float)
    float *data;
};

struct VBO {
    VBO() : total_size(0) {}
    int total_size; // num of float
    std::vector<Component> format;
};

struct EBO {
    EBO() : count(0), ind(NULL) {}
    int count;
    int* ind;
};

std::vector<VBO> vbos;
std::vector<EBO> ebos;

int mgl_create_vbo() {
    vbos.emplace_back();
    return (int)vbos.size() - 1;
}

int mgl_create_ebo() {
    ebos.emplace_back();
    return (int)ebos.size() - 1;
}

float* mgl_query_vbo(int vbo_ind, int index, int location, int* size) {
    if(vbo_ind < 0 || vbo_ind >= (int)vbos.size()) return NULL;
    VBO& vbo = vbos[vbo_ind];
    if(location < 0 || location >= (int)vbo.format.size()) return NULL;
    Component& comp = vbo.format[location];
    if(size) *size = comp.size;
    return comp.data + (*size) * index;
}

// size: num of float
int mgl_vertex_attrib_pointer(int vbo_ind, int size, float* data) {
    if(vbo_ind < 0 || vbo_ind >= (int)vbos.size()) return -1;
    VBO &vbo = vbos[vbo_ind];
    vbo.format.emplace_back(size, data);
    vbo.total_size += size;
    return (int)vbo.format.size() - 1;
}

int mgl_vertex_index_pointer(int ebo_ind, int count, int* ind) {
    if(ebo_ind < 0 || ebo_ind >= (int)vbos.size()) return -1;
    EBO &ebo = ebos[ebo_ind];
    ebo.ind = ind;
    ebo.count = count;
    return count;
}


void mgl_init(const char *title, int w, int h) {
    SDL_Init(SDL_INIT_VIDEO);
    width = w;
    height = h;
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
    buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    zbuffer.resize(w * h);
    SDL_LockTexture(buffer, NULL, (void**)&pixels, &pitch);
    if(title) SDL_SetWindowTitle(window, title);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_RENDERER_ACCELERATED);
    SDL_RenderClear(renderer);
}


void set_pixel(int x, int y, vec4 color) {
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);
    pixels[(height - y - 1) * width + x] = packRGBA8888(color);
}

void test_and_set_pixel(int x, int y, vec4 color, float depth) {
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);
    if(zbuffer[y * width + x] > depth) {
        set_pixel(x, y, color);
        zbuffer[y * width + x] = depth;
    }
}

float calc_edge_dis(const vec2& v1, const vec2& v2, const vec2& point) {
    double x = point.x(), y = point.y();
    return x * (v1.y() - v2.y()) + y * (v2.x() - v1.x()) + ((double)v1.x() * v2.y() - (double)v2.x() * v1.y());
}

void rasterize(Tr_element& tr, Shader* shader) {
    float r_w_a = 1.0f / tr.points[0].w();
    float r_w_b = 1.0f / tr.points[1].w();
    float r_w_c = 1.0f / tr.points[2].w();

    float z_a = tr.points[0].z() * r_w_a;
    float z_b = tr.points[1].z() * r_w_b;
    float z_c = tr.points[2].z() * r_w_c;

    mat4 screen_mat(width / 2.0f, 0.0f, 0.0f, width / 2.0f,
                    0.0f, height / 2.0f, 0.0f, height / 2.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);
    
    vec2 a = (screen_mat * (tr.points[0] * r_w_a)).e;
    vec2 b = (screen_mat * (tr.points[1] * r_w_b)).e;
    vec2 c = (screen_mat * (tr.points[2] * r_w_c)).e;
    
    // 背面剔除
    // if(cross(b - a, c - a) < 0.0f) return ;

    float fa = calc_edge_dis(b, c, a);
    float fb = calc_edge_dis(c, a, b);
    float fc = calc_edge_dis(a, b, c);

    float nfa = calc_edge_dis(b, c, vec2(-1.0f, -1.0f));
    float nfb = calc_edge_dis(c, a, vec2(-1.0f, -1.0f));
    float nfc = calc_edge_dis(a, b, vec2(-1.0f, -1.0f));

    const float eps = 1e-5;
    if(fabs(fa) < eps || fabs(fb) < eps || fabs(fc) < eps) return ;
        
    auto shade = [&](vec2 p, vec4& color, float& z) -> bool {
        float alpha = calc_edge_dis(b, c, p) / fa;
        float beta  = calc_edge_dis(c, a, p) / fb;
        float gamma = calc_edge_dis(a, b, p) / fc;
        
        if(alpha < 0.0f || beta < 0.0f || gamma < 0.0f) return false;
        if( (alpha > 0.0f || fa * nfa > 0.0f) &&
            (beta  > 0.0f || fb * nfb > 0.0f) &&
            (gamma > 0.0f || fc * nfc > 0.0f)) {

            floatstream int_varying;
            float rone = 1.0f / (alpha * r_w_a + beta * r_w_b + gamma * r_w_c);
            for(int k = 0; k < tr.varyings[0].size(); k++) {
                float v =   alpha * tr.varyings[0][k] * r_w_a + 
                            beta  * tr.varyings[1][k] * r_w_b + 
                            gamma * tr.varyings[2][k] * r_w_c;
                int_varying.push_back(v * rone);
            }
            z = alpha * z_a + beta * z_b + gamma * z_c;
            z = (z + 1.0f) * 0.5f;
            color = shader->fragment_shader(int_varying);
            return true;
        }
        return false;
    };

    auto min3f = [](float a, float b, float c) {return std::min(a, std::min(b, c));};
    auto max3f = [](float a, float b, float c) {return std::max(a, std::max(b, c));};
    int xl = floor(min3f(a.x(), b.x(), c.x())), yl = floor(min3f(a.y(), b.y(), c.y()));
    int xr = ceil(max3f(a.x(), b.x(), c.x())), yr = ceil(max3f(a.y(), b.y(), c.y()));
    xl = std::max(0, xl);
    yl = std::max(0, yl);
    xr = std::min(xr, width - 1);
    yr = std::min(yr, height - 1);   

    for(int i = yl; i <= yr; i++) {
        for(int j = xl; j <= xr; j++) {
            float z;
            vec4 color;
            if(shade(vec2(j + 0.5f, i + 0.5f), color, z)) {
                test_and_set_pixel(j, i, color, z);
            }
        }
    }
    
}

void clip(Tr_element &tr, std::vector<Tr_element>& triangles) {
    vec4 &a = tr.points[0];
    vec4 &b = tr.points[1];
    vec4 &c = tr.points[2];
    floatstream &va = tr.varyings[0];
    floatstream &vb = tr.varyings[1];
    floatstream &vc = tr.varyings[2];

    bool c_a = a.w() < -a.z();
    bool c_b = b.w() < -b.z();
    bool c_c = c.w() < -c.z();

    if(c_a && c_b && c_c) {
        return ;
    }
    if(!c_a && !c_b && !c_c) {
        triangles.emplace_back(tr);
        return ;
    }
    bool dif = c_a ^ c_b ^ c_c;
    if(c_b == dif) {
        std::swap(a, c);
        std::swap(a, b);
        std::swap(va, vc);
        std::swap(va, vb);
    } else if(c_c == dif) {
        std::swap(a, b);
        std::swap(a, c);
        std::swap(va, vb);
        std::swap(va, vc);
    }
    float t1 = (a.w() + a.z()) / ((a.z() + a.w()) - (b.z() + b.w()));
    float t2 = (a.w() + a.z()) / ((a.z() + a.w()) - (c.z() + c.w()));
    vec4 pab = a + t1 * (b - a);
    vec4 pac = a + t2 * (c - a);
    floatstream vab, vac;
    for(int i = 0; i < va.size(); i++) {
        vab.push_back(va[i] + t1 * (vb[i] - va[i]));
        vac.push_back(va[i] + t2 * (vc[i] - va[i]));
    }
    if(dif) {
        triangles.emplace_back(Tr_element{{pab, b, c}, {vab, vb, vc}});
        triangles.emplace_back(Tr_element{{pab, c, pac}, {vab, vc, vac}});
    } else {
        triangles.emplace_back(Tr_element{{a, pab, pac}, {va, vab, vac}});
    }
}

void mgl_draw(int vbo_ind, int ebo_ind, Shader* shader) {
    if(vbo_ind < 0 || vbo_ind >= (int)vbos.size()) return ;
    if(ebo_ind < 0 || ebo_ind >= (int)ebos.size()) return ;
    
    EBO& ebo = ebos[ebo_ind];
    std::vector<Tr_element> triangles;

    for(int i = 0; i < ebo.count; i += 3) {
        Tr_element tr;
        for(int j = 0; j < 3; j++) {
            int ind = i + j;
            if(ebo.ind) ind = ebo.ind[i + j];
            tr.points[j] = shader->vertex_shader(vbo_ind, ind, tr.varyings[j]);
        }
        clip(tr, triangles);       
    }
    
    int tr_count = triangles.size();

    for(int i = 0; i < tr_count; i++) {
        rasterize(triangles[i], shader);
    }
}

void mgl_set_init_zbuffer(float z) {
    init_z = z;
}

void mgl_set_init_color(vec4 col) {
    init_col = col;
}
 
void mgl_clear(int flag) {
    assert(width * 4 == pitch);
    if(flag & MGL_COLOR) {
        for(int i = 0; i < height; i++) {
            for(int j = 0; j < width; j++) {
                pixels[i * width + j] = packRGBA8888(init_col);
            }
        }
    }
    if(flag & MGL_DEPTH) {
        std::fill(zbuffer.begin(), zbuffer.end(), init_z);
    }
}

void mgl_update() {
    SDL_UnlockTexture(buffer);
    SDL_RenderCopy(renderer, buffer, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_LockTexture(buffer, NULL, (void**)&pixels, &pitch);
}

void mgl_quit() {
    SDL_DestroyTexture(buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}