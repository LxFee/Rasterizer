#include <cassert>
#include <vector>
#include <cmath> 
#include <iostream>
#include "graphics.h"

vbo_t::vbo_t(int _sizeof_element, int _count) 
    : sizeof_element(_sizeof_element), count(_count), raw_data(NULL) {
    raw_data = new char[_sizeof_element * _count];
}

vbo_t::~vbo_t() {
    delete [] raw_data;
}

void* vbo_t::data() {
    return raw_data;
}

const void* vbo_t::data() const {
    return raw_data;
}

void* vbo_t::at(int p) {
    assert(p >= 0 && p < count);
    return ((char*)raw_data) + p * sizeof_element;
}

const void* vbo_t::at(int p) const {
    assert(p >= 0 && p < count);
    return ((char*)raw_data) + p * sizeof_element;
}

int vbo_t::get_sizeof_element() const {
    return sizeof_element;
}

int vbo_t::get_count() const {
    return count;
}

int vbo_t::get_totol_size() const {
    return get_count() * get_sizeof_element();
}

framebuffer_t::framebuffer_t(int _width, int _height) 
    : width(_width), height(_height), color_buffer(NULL), depth_buffer(NULL) {
    color_buffer = new uchar[width * height * 4];
    depth_buffer = new float[width * height];
}

framebuffer_t::~framebuffer_t() {
    delete [] color_buffer;
    delete [] depth_buffer;
}

int framebuffer_t::get_width() const {
    return width;
}
int framebuffer_t::get_height() const {
    return height;
}

void framebuffer_t::clear_color(vec4 color) {
    for(int i = 0; i < width * height; i++) {
        ((uint*)color_buffer)[i] = rgba2rgbapack(color);
    }
}

void framebuffer_t::clear_depth(float depth) {
    for(int i = 0; i < width * height; i++) {
        depth_buffer[i] = depth;
    }
}

float framebuffer_t::get_depth(int x, int y) const {
    assert(x >= 0 && x < width && y >= 0 && y < height);
    int p = y * width + x;
    return depth_buffer[p];
}

const vec4 framebuffer_t::get_color(int x, int y) const {
    assert(x >= 0 && x < width && y >= 0 && y < height);
    int p = (y * width + x) * 4;
    return rgbapack2rgba(color_buffer + p);
}

void framebuffer_t::set_depth(int x, int y, float depth) {
    assert(x >= 0 && x < width && y >= 0 && y < height);
    int p = y * width + x;
    depth_buffer[p] = depth;
}

void framebuffer_t::set_color(int x, int y, vec4 color) {
    assert(x >= 0 && x < width && y >= 0 && y < height);
    int p = y * width + x;
    ((uint*)color_buffer)[p] = rgba2rgbapack(color);
}

const uchar *framebuffer_t::get_color_data() const {
    return color_buffer;
}

const float *framebuffer_t::get_color_depth() const {
    return depth_buffer;
}



namespace {
    namespace render {
        struct v2f_t {
            v2f_t(int _sizeof_varing) {
                data = new char[_sizeof_varing];
                sizeof_varying = _sizeof_varing;
            }
            ~v2f_t() {
                delete [] data;
            }
            v2f_t(const v2f_t&) = delete;
            v2f_t &operator = (const v2f_t&) = delete;
            
            vec4 position;
            int sizeof_varying;
            char* data;
        };

        void lerp_v2f(const v2f_t* a, const v2f_t* b, float t, v2f_t* target) {
            target->position = a->position + (b->position - a->position) * t;
            int count = (a->sizeof_varying) >> 2;
            float *data_a = (float*)a->data;
            float *data_b = (float*)b->data;
            float *data_c = (float*)target->data;
            for(int i = 0; i < count; i++) {
                data_c[i] = data_a[i] + (data_b[i] - data_a[i]) * t;
            }
        }

        void interpolation_v2f(const v2f_t* a, const v2f_t* b, const v2f_t* c, float alpha, float beta, float gamma, v2f_t* target) {
            float weight = 1.0 / (alpha + beta + gamma);
            target->position = (a->position * alpha + b->position * beta + c->position * gamma) * weight;
            float *data_target = (float *)target->data;
            float *data_a = (float *)a->data;
            float *data_b = (float *)b->data;
            float *data_c = (float *)c->data;
            int count = (a->sizeof_varying) >> 2;
            for(int i = 0; i < count; i++) {
                data_target[i] = (alpha * data_a[i] + beta * data_b[i] + gamma * data_c[i]) * weight;
            }
        }

        void clip_aganst_panels(v2f_t* v2fs[3], std::vector<v2f_t*>& vertexes, std::vector<int>& indexes) {
            const static vec4 planes[6] {
                vec4(0, 0, 1, 1), // near
                vec4(0, 0, -1, 1), // far
                vec4(1, 0, 0, 1), // left
                vec4(-1, 0, 0, 1), // right
                vec4(0, 1, 0, 1), // top
                vec4(0, -1, 0, 1) // bottom
            };

            int sizeof_varying = v2fs[0]->sizeof_varying; 
            std::vector<v2f_t*> vertex_list;
            std::vector<int> input;
            for(int i = 0; i < 3; i++) {
                input.push_back(i);
                vertex_list.push_back(v2fs[i]);
                v2fs[i] = NULL;
            }
            for(int i = 0; i < 6; i++) {
                const vec4 &C = planes[i];
                if(input.empty()) break;
                std::vector<int> output;
                int p = 0, s = input.size() - 1;

                while(p < input.size()) {
                    int pp = input[p], sp = input[s];
                    float d1 = vertex_list[pp]->position.dot(C);
                    float d2 = vertex_list[sp]->position.dot(C);
                    int situation = ((d1 >= 0) | ((d2 >= 0) << 1));

                    if(situation == 0) {
                        // do nothing
                    }
                    else if(situation == 1) {
                        if(fabs(d1 - d2) > 1e-5) {
                            vertex_list.push_back(new v2f_t(sizeof_varying));
                            lerp_v2f(vertex_list[pp], vertex_list[sp], d1 / (d1 - d2), vertex_list.back());
                            output.push_back(vertex_list.size() - 1);
                        }
                        output.push_back(pp);
                    }
                    else if(situation == 2) {
                        if(fabs(d1 - d2) > 1e-5) {
                            vertex_list.emplace_back();
                            lerp_v2f(vertex_list[pp], vertex_list[sp], d1 / (d1 - d2), vertex_list.back());
                            output.push_back(vertex_list.size() - 1);
                        }
                    }
                    else if(situation == 3) {
                        output.push_back(pp);
                    }
                    else {
                        assert(0);
                    }
                    s = p;
                    p++;
                }
                input.swap(output);
            }

            if(input.size() <= 2) {
                for(int i = 0; i < vertex_list.size(); i++) {
                    delete vertex_list[i];
                }
                return ;
            }

            
            int ind = vertexes.size();
            for(int i = 1; i + 1 < input.size(); i++) {
                indexes.push_back(ind);
                indexes.push_back(ind + i);
                indexes.push_back(ind + i + 1);
            }
            for(int i = 0; i < input.size(); i++) {
                int p = input[i];
                vertexes.push_back(vertex_list[p]);
                vertex_list[p] = NULL;
            }
            for(int i = 0; i < vertex_list.size(); i++) {
                if(!vertex_list[i]) continue;
                delete vertex_list[i];
            }
        }

        bool back_face(vec2 a, vec2 b) {
            float det = a.x() * b.y() - b.x() * a.y();
            return det < 0.0f;
        }

        float calc_line_distance(const vec2& v1, const vec2& v2, const vec2& point) {
            return point.x() * (v1.y() - v2.y()) + point.y() * (v2.x() - v1.x()) + (v1.x() * v2.y() - v2.x() * v1.y());
        }

        void rasterize(framebuffer_t *framebuffer, const v2f_t* a, const v2f_t* b, const v2f_t* c, shader_t* shader) {
            float raw = 1.0f / a->position.w();
            float rbw = 1.0f / b->position.w();
            float rcw = 1.0f / c->position.w();

            float az = a->position.z() * raw;
            float bz = b->position.z() * rbw;
            float cz = c->position.z() * rcw;

            int width = framebuffer->get_width();
            int height = framebuffer->get_height();
            
            mat4 viewport_mat = viewport(width, height);
            vec4 pa3 = viewport_mat.mul_vec4(a->position * raw);
            vec4 pb3 = viewport_mat.mul_vec4(b->position * rbw);
            vec4 pc3 = viewport_mat.mul_vec4(c->position * rcw);
            
            vec2 pa2(pa3.x(), pa3.y());
            vec2 pb2(pb3.x(), pb3.y());
            vec2 pc2(pc3.x(), pc3.y());
            
            // 背面剔除
            if(back_face(pb2 - pa2, pc2 - pa2)) return ;

            float da = calc_line_distance(pb2, pc2, pa2);
            float db = calc_line_distance(pc2, pa2, pb2);
            float dc = calc_line_distance(pa2, pb2, pc2);
            if(fabs(da) < EPSILON || fabs(db) < EPSILON || fabs(dc) < EPSILON) return ;

            auto min3f = [](float a, float b, float c) {return std::min(a, std::min(b, c));};
            auto max3f = [](float a, float b, float c) {return std::max(a, std::max(b, c));};
            int xl = floor(min3f(pa2.x(), pb2.x(), pc2.x()));
            int yl = floor(min3f(pa2.y(), pb2.y(), pc2.y()));
            int xr = ceil (max3f(pa2.x(), pb2.x(), pc2.x()));
            int yr = ceil (max3f(pa2.y(), pb2.y(), pc2.y()));
            xl = std::max(0, xl);
            yl = std::max(0, yl);
            xr = std::min(xr, width - 1);
            yr = std::min(yr, height - 1);   
            
            v2f_t v2f(a->sizeof_varying);
            for(int i = yl; i <= yr; i++) {
                for(int j = xl; j <= xr; j++) {
                    vec2 p(j + 0.5f, i + 0.5f);
                    float alpha = calc_line_distance(pb2, pc2, p) / da;
                    float beta  = calc_line_distance(pc2, pa2, p) / db;
                    float gamma = calc_line_distance(pa2, pb2, p) / dc;

                    if(alpha < 0.0f || beta < 0.0f || gamma < 0.0f) continue;
                    
                    float depth = alpha * az + beta * bz + gamma * cz;
                    depth = (depth + 1.0f) * 0.5f;

                    if(framebuffer->get_depth(j, i) < depth) continue;
                    
                    interpolation_v2f(a, b, c, alpha * raw, beta * rbw, gamma * rcw, &v2f);
                    bool discord = false;
                    vec4 color = shader->fragment_shader(v2f.data, discord);
                    if(discord) continue;
                    framebuffer->set_depth(j, i, depth);
                    framebuffer->set_color(j, i, color);
                }
            }
            
        }
    }
}

void draw_triangle(framebuffer_t* framebuffer, const vbo_t* data, shader_t* shader) {
    assert(framebuffer && data && shader);
    using namespace render;

    int sizeof_element = data->get_sizeof_element();
    int count = data->get_count();

    std::vector<v2f_t*> vertexes;
    std::vector<int> indexes;

    for(int i = 0; i < count; i += 3) {
        v2f_t* v2fs[3];
        for(int j = 0; j < 3; j++) {
            v2fs[j] = new v2f_t(shader->get_sizeof_varyings());
            int ind = i + j;
            vec4 position = shader->vertex_shader(data->at(ind), v2fs[j]->data);
            v2fs[j]->position = position;
        }
        clip_aganst_panels(v2fs, vertexes, indexes);
    }

    for(int i = 0; i < indexes.size(); i += 3) {
        v2f_t* a = vertexes[indexes[i]];
        v2f_t* b = vertexes[indexes[i + 1]];
        v2f_t* c = vertexes[indexes[i + 2]];
        rasterize(framebuffer, a, b, c, shader);
    }
    for(int i = 0; i < vertexes.size(); i++) {
        assert(vertexes[i]);
        delete vertexes[i];
    }
}