#include "mgl.h"

#include <cmath>
#include <cstdio>
#include <memory>
#include <iostream>

#include "model.h"

namespace {
    struct Tr_element {
        vec4 points[3];
        floatstream varyings[3];
    };

    struct VBO {
        VBO(int el_size, const float* _data, int cnt) 
            : size(el_size), format(), data(), count(cnt) {
                if(_data) {
                    int total = (el_size * cnt) >> 2;
                    std::shared_ptr<float> new_data(new float[total], [](float *p){delete [] p;});
                    data = new_data;
                    memcpy(data.get(), _data, total << 2);
                }
            }
        int size;
        std::vector<int> format;
        std::shared_ptr<float> data;
        int count;
    };

    struct EBO {
        EBO(const int* _data, int cnt) 
            : data(), count(cnt) {
                if(_data) {
                    std::shared_ptr<int> new_data(new int[cnt], [](int *p){delete [] p;});
                    data = new_data;
                    memcpy(data.get(), _data, sizeof(int) * cnt);
                }
            }
        std::shared_ptr<int> data;
        int count;
    };

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* buffer;
    int width, height;
    int* pixels;

    vec4 clear_color;
    float clear_depth;
    std::vector<float> zbuffer;

    std::vector<VBO> vbos;
    std::vector<EBO> ebos;


    void set_pixel(int x, int y, vec4 color, float depth) {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        pixels[(height - y - 1) * width + x] = packRGBA8888(color);
        zbuffer[y * width + x] = depth;
    }

    bool test(int x, int y, float depth) {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        return zbuffer[y * width + x] >= depth;
    }

    inline float calc_edge_dis(const vec2& v1, const vec2& v2, const vec2& point) {
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
        if(det(b - a, c - a) < 0.0f) return ;

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
                z = alpha * z_a + beta * z_b + gamma * z_c;
                z = (z + 1.0f) * 0.5f;
                if(!test(p.x(), p.y(), z)) return false;
                floatstream int_varying;
                float rone = 1.0f / (alpha * r_w_a + beta * r_w_b + gamma * r_w_c);
                for(int k = 0; k < tr.varyings[0].size(); k++) {
                    float v =   alpha * tr.varyings[0][k] * r_w_a + 
                                beta  * tr.varyings[1][k] * r_w_b + 
                                gamma * tr.varyings[2][k] * r_w_c;
                    int_varying.push_back(v * rone);
                }
                
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
                    set_pixel(j, i, color, z);
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
}

void mgl_init(const char *title, int w, int h) {
    assert(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) == 0);
    width = w;
    height = h;
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, window_flags);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    zbuffer.resize(width * height);
    int pitch;
    SDL_LockTexture(buffer, NULL, (void**)&pixels, &pitch);
    assert(4 * width == pitch);
    if(title) SDL_SetWindowTitle(window, title);
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);

    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void mgl_quit() {
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyTexture(buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void mgl_draw(int vbo_ind, int ebo_ind, Shader* shader) {
    if(vbo_ind < 0 || vbo_ind >= (int)vbos.size()) return ;
    if(ebo_ind >= (int)ebos.size()) return ;
    
    VBO& vbo = vbos[vbo_ind];
    std::vector<Tr_element> triangles;
    
    int* indexes = NULL;
    int count = vbo.count;
    if(ebo_ind >= 0) { 
        count = ebos[ebo_ind].count;
        indexes = ebos[ebo_ind].data.get();
    }

    for(int i = 0; i < count; i += 3) {
        Tr_element tr;
        for(int j = 0; j < 3; j++) {
            int ind;
            if(indexes) ind = indexes[i + j];
            else ind = i + j;
            tr.points[j] = shader->vertex_shader((const float*)((const char*)vbo.data.get() + ind * vbo.size), vbo.format, tr.varyings[j]);
        }
        clip(tr, triangles);       
    }
    
    int tr_count = triangles.size();

    for(int i = 0; i < tr_count; i++) {
        rasterize(triangles[i], shader);
    }
}


void mgl_clear_depth(float depth) {
    clear_depth = depth;
}

void mgl_clear_color(vec4 col) {
    clear_color = col;
}
 
void mgl_clear(int flag) {
    if(flag & MGL_COLOR) {
        for(int i = 0; i < height; i++) {
            for(int j = 0; j < width; j++) {
                pixels[i * width + j] = packRGBA8888(clear_color);
            }
        }
    }
    if(flag & MGL_DEPTH) {
        std::fill(zbuffer.begin(), zbuffer.end(), clear_depth);
    }
}


bool mgl_update() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if(event.type == SDL_QUIT)
            return true;
        if( event.type == SDL_WINDOWEVENT && 
            event.window.event == SDL_WINDOWEVENT_CLOSE && 
            event.window.windowID == SDL_GetWindowID(window)) {
            return true;
        }
    }
    
    SDL_UnlockTexture(buffer);
    int pitch;
    SDL_LockTexture(buffer, NULL, (void**)&pixels, &pitch);
    SDL_RenderCopy(renderer, buffer, NULL, NULL);
    
    ImGui::Render();
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(renderer);
    
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    return false;
}

int mgl_create_vbo(int size, const void* data, int count) {
    assert(data);
    vbos.emplace_back(size, (const float*)data, count);
    return (int)vbos.size() - 1;
}

int mgl_create_ebo(const int* data, int count) {
    assert(data);
    ebos.emplace_back(data, count);
    return (int)ebos.size() - 1;
}

int mgl_vertex_attrib_pointer(int vbo_ind, int location, int size, int offset) {
    if(vbo_ind < 0 || vbo_ind >= (int)vbos.size()) return -1;
    if(location < 0) return -1;
    VBO &vbo = vbos[vbo_ind];
    if(location + 1 >= vbo.format.size())
        vbo.format.resize(location + 2, 0);
    vbo.format[location] = offset;
    vbo.format[location + 1] = offset + size;
    return location;
}