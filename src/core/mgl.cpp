#include "mgl.h"

#include <cmath>
#include <cstdio>
#include <cassert>
#include <memory>
#include <iostream>
#include <vector>

#include "mathbase.h"
#include "Shader.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

namespace mgltexture {
    namespace {
        vec4 bilinear(vec4 i00, vec4 i10, vec4 i01, vec4 i11, float u, float v) {
            vec4 i0 = i00 + u * (i10 - i00);
            vec4 i1 = i01 + u * (i11 - i01);
            return i0 + v * (i1 - i0);
        }
    }

    struct Texture {
        Texture(int w, int h)
        : w(w), 
        h(h), 
        data(), 
        surround(FILLED), 
        interpolation(BILINEAR), 
        filled_color(0.0f) 
        {        
            data.resize(w * h);
        }

        vec4 sample(float u, float v) const {
            if(u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f) {
                if(surround == FILLED) {
                    return filled_color;
                }
                if(surround == REPEAT) {
                    u = u - floor(u);
                    v = v - floor(v);
                }
            }
            if(interpolation == NEAREST) {
                int x = std::min(int(u * w), w - 1);
                int y = std::min(int(v * h), h - 1);
                return data[y * w + x];
            }
            if(interpolation == BILINEAR) {
                int x = u * (w - 1);
                int y = v * (h - 1);
                if(x == w - 1 || y == h - 1) return data[y * w + x];
                float local_u = u * (w - 1) - x; 
                float local_v = v * (h - 1) - y;
                return bilinear(data[y * w + x], data[y * w + x + 1], data[y * w + x + w], data[y * w + x + w + 1], local_u, local_v);
            }
            return vec4(0.0f);
        }

        int w, h;
        std::vector<vec4> data;
        SURROUND surround;
        INTERPOLATION interpolation;
        vec4 filled_color;
    };

    inline std::vector<const mgltexture::Texture*>& active_textures() {
        static std::vector<const mgltexture::Texture*> active_texture_entities;
        return active_texture_entities;
    }    
}

namespace {

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

    inline std::vector<VBO>& vbos() {
        static std::vector<VBO> vbo_entities;
        return vbo_entities;
    }

    inline std::vector<EBO>& ebos() {
        static std::vector<EBO> ebo_entities;
        return ebo_entities;
    }

    inline std::vector<mgltexture::Texture>& texs() {
        static std::vector<mgltexture::Texture> texture_entities;
        return texture_entities;
    }

    void lerp(const Shader::V2FO& a, const Shader::V2FO& b, float t, Shader::V2FO& c) {
        c.position = a.position + t * (b.position - a.position);
        c.texcoord = a.texcoord + t * (b.texcoord - a.texcoord);
        c.varying.resize(a.varying.size());
        for(int i = 0; i < a.varying.size(); i++) {
            c.varying[i] = a.varying[i] + t * (b.varying[i] - a.varying[i]);
        }
    }

    void interpolation( const Shader::V2FO& a, const Shader::V2FO& b, const Shader::V2FO& c, 
                        float alpha, float beta, float gamma, Shader::V2FI& d, bool correction) {
        float weight = 1.0f;
        if(correction) weight = 1.0 / (alpha + beta + gamma);
        d.position = (alpha * a.position + beta * b.position + gamma * c.position) * weight;
        d.texcoord = (alpha * a.texcoord + beta * b.texcoord + gamma * c.texcoord) * weight;
        d.varying.resize(a.varying.size());
        for(int i = 0; i < a.varying.size(); i++) {
            d.varying[i] = (alpha * a.varying[i] + beta * b.varying[i] + gamma * c.varying[i]) * weight;
        }
    }

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

    void rasterize(const Shader::V2FO& a, const Shader::V2FO& b, const Shader::V2FO& c, Shader* shader) {
        float r_w_a = 1.0f / a.position.w();
        float r_w_b = 1.0f / b.position.w();
        float r_w_c = 1.0f / c.position.w();

        float z_a = a.position.z() * r_w_a;
        float z_b = b.position.z() * r_w_b;
        float z_c = c.position.z() * r_w_c;

        mat4 screen_mat(width / 2.0f, 0.0f, 0.0f, width / 2.0f,
                        0.0f, height / 2.0f, 0.0f, height / 2.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f);
        
        vec2 pa((screen_mat * (a.position * r_w_a)).data());
        vec2 pb((screen_mat * (b.position * r_w_b)).data());
        vec2 pc((screen_mat * (c.position * r_w_c)).data());
        
        // std::cout << pa.x() << " " << pa.y() << std::endl;
        // std::cout << pb.x() << " " << pb.y() << std::endl;
        // std::cout << pc.x() << " " << pc.y() << std::endl;
        // std::cout << std::endl;
        
        // 背面剔除
        if(det(pb - pa, pc - pa) < 0.0f) return ;

        float fa = calc_edge_dis(pb, pc, pa);
        float fb = calc_edge_dis(pc, pa, pb);
        float fc = calc_edge_dis(pa, pb, pc);

        const float eps = 1e-5;
        if(fabs(fa) < eps || fabs(fb) < eps || fabs(fc) < eps) return ;
            
        auto shade = [&](vec2 p, vec4& color, float& z) -> bool {
            float alpha = calc_edge_dis(pb, pc, p) / fa;
            float beta  = calc_edge_dis(pc, pa, p) / fb;
            float gamma = calc_edge_dis(pa, pb, p) / fc;

            if(alpha < 0.0f || beta < 0.0f || gamma < 0.0f) return false;
            z = alpha * z_a + beta * z_b + gamma * z_c;
            z = (z + 1.0f) * 0.5f;
            if(!test(p.x(), p.y(), z)) return false;
            Shader::V2FI v2f;
            Shader::F2B f2b;

            interpolation(a, b, c, alpha * r_w_a, beta * r_w_b, gamma * r_w_c, v2f, true);
            
            shader->fragment_shader(v2f, f2b);
            color = f2b.color0;
            return true;
        };

        auto min3f = [](float a, float b, float c) {return std::min(a, std::min(b, c));};
        auto max3f = [](float a, float b, float c) {return std::max(a, std::max(b, c));};
        int xl = floor(min3f(pa.x(), pb.x(), pc.x()));
        int yl = floor(min3f(pa.y(), pb.y(), pc.y()));
        int xr = ceil(max3f(pa.x(), pb.x(), pc.x()));
        int yr = ceil(max3f(pa.y(), pb.y(), pc.y()));
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

    void clip(Shader::V2FO v2fs[3], std::vector<Shader::V2FO>& triangles) {
        Shader::V2FO &a = v2fs[0];
        Shader::V2FO &b = v2fs[1];
        Shader::V2FO &c = v2fs[2];

        bool c_a = a.position.w() < -a.position.z();
        bool c_b = b.position.w() < -b.position.z();
        bool c_c = c.position.w() < -c.position.z();

        if(c_a && c_b && c_c) {
            return ;
        }
        if(!c_a && !c_b && !c_c) {
            triangles.emplace_back(v2fs[0]);
            triangles.emplace_back(v2fs[1]);
            triangles.emplace_back(v2fs[2]);
            return ;
        }
        bool dif = c_a ^ c_b ^ c_c;
        if(c_b == dif) {
            std::swap(a, c);
            std::swap(a, b);
        } else if(c_c == dif) {
            std::swap(a, b);
            std::swap(a, c);
        }
        float t1 = (a.position.w() + a.position.z()) / ((a.position.z() + a.position.w()) - (b.position.z() + b.position.w()));
        float t2 = (a.position.w() + a.position.z()) / ((a.position.z() + a.position.w()) - (c.position.z() + c.position.w()));
        
        Shader::V2FO pab, pac;
        lerp(a, b, t1, pab);
        lerp(a, c, t2, pac);

        if(dif) {
            triangles.emplace_back(pab);
            triangles.emplace_back(b);
            triangles.emplace_back(c);
            
            triangles.emplace_back(pab);
            triangles.emplace_back(c);
            triangles.emplace_back(pac);
            
        } else {
            triangles.emplace_back(a);
            triangles.emplace_back(pab);
            triangles.emplace_back(pac);
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
    if(vbo_ind < 0 || vbo_ind >= (int)vbos().size()) return ;
    if(ebo_ind >= (int)ebos().size()) return ;
    
    VBO& vbo = vbos()[vbo_ind];
    std::vector<Shader::V2FO> triangles;
    
    int* indexes = NULL;
    int count = vbo.count;
    if(ebo_ind >= 0) { 
        count = ebos()[ebo_ind].count;
        indexes = ebos()[ebo_ind].data.get();
    }
    
    for(int i = 0; i < count; i += 3) {
        Shader::V2FO v2fs[3];
        for(int j = 0; j < 3; j++) {
            int ind;
            if(indexes) ind = indexes[i + j];
            else ind = i + j;
            shader->vertex_shader((const float*)((const char*)vbo.data.get() + ind * vbo.size), vbo.format, v2fs[j]);
            // std::cout << v2fs[j].position.x() << " " << v2fs[j].position.y() << " " << v2fs[j].position.z() << std::endl;
        }
        // std::cout << std::endl;
        clip(v2fs, triangles);
    }

    int tr_count = triangles.size();
    for(int i = 0; i < tr_count; i += 3) {
        rasterize(triangles[i], triangles[i + 1], triangles[i + 2], shader);
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
    vbos().emplace_back(size, (const float*)data, count);
    return (int)vbos().size() - 1;
}

int mgl_create_ebo(const int* data, int count) {
    assert(data);
    ebos().emplace_back(data, count);
    return (int)ebos().size() - 1;
}

int mgl_vertex_attrib_pointer(int vbo_ind, int location, int size, int offset) {
    if(vbo_ind < 0 || vbo_ind >= (int)vbos().size()) return -1;
    if(location < 0) return -1;
    VBO &vbo = vbos()[vbo_ind];
    if(location + 1 >= vbo.format.size())
        vbo.format.resize(location + 2, 0);
    vbo.format[location] = offset;
    vbo.format[location + 1] = offset + size;
    return location;
}

int mgl_gen_texture(int w, int h) {
    auto &texture_entities = texs();
    texture_entities.emplace_back(w, h);
    return (int)texture_entities.size() - 1;
}

int mgl_gen_texture_image(int w, int h, int wide, const unsigned char* data) {
    if (!data) return -1;
    if(wide != 3 && wide != 4) return -1;
    auto &texture_entities = texs();
    texture_entities.emplace_back(w, h);
    int texture_id = (int)texture_entities.size() - 1;
    if(wide == 3) {
        for(int i = 0; i < h; i++) {
            for(int j = 0; j < w; j++) {
                texture_entities[texture_id].data[(h - i - 1) * w + j] = unpackRGBA888(data + 3 * (i * w + j));
            }
        }
    } else { // nrChannels == 4
        for(int i = 0; i < h; i++) {
            for(int j = 0; j < w; j++) {
                texture_entities[texture_id].data[(h - i - 1) * w + j] = unpackRGBA8888(data + 4 * (i * w + j));
            }
        }
    }
    return texture_id;
}

void mgl_active_texture(int texture_id, int texture_location) {
    auto &texture_entities = texs();
    auto &active_texture_entities = mgltexture::active_textures();
    if(texture_location < 0) return ;
    if(texture_id >= texture_entities.size() || texture_id < 0) return ;
    if(texture_location >= active_texture_entities.size()) {
        active_texture_entities.resize(texture_location + 1, NULL);
    }
    active_texture_entities[texture_location] = &(texs()[texture_id]);
}

void mgl_texture_parameteri(int texture_id, mgltexture::SURROUND surr, mgltexture::INTERPOLATION intp) {
    auto &texture_entities = texs();
    if(texture_id >= texture_entities.size() || texture_id < 0) return ;
    texs()[texture_id].surround = surr;
    texs()[texture_id].interpolation = intp;
}

void mgl_texture_parameterv(int texture_id, vec4 filled_color) {
    auto &texture_entities = texs();
    if(texture_id >= texture_entities.size() || texture_id < 0) return ;
    texs()[texture_id].filled_color = filled_color;
}

const vec4 mgl_texture_sample2d(int texture_location, float u, float v) {
    auto &active_texture_entities = mgltexture::active_textures();
    if(texture_location < 0 || texture_location >= active_texture_entities.size()) {
        return vec4(0.0f);
    }
    return active_texture_entities[texture_location]->sample(u, v);
}