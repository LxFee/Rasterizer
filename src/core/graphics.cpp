#include <cassert>
#include <vector>
#include <cmath> 
#include <iostream>
#include <memory>
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
    int p = (height - y - 1) * width + x;
    return depth_buffer[p];
}

const vec4 framebuffer_t::get_color(int x, int y) const {
    assert(x >= 0 && x < width && y >= 0 && y < height);
    int p = ((height - y - 1) * width + x) * 4;
    return rgbapack2rgba(color_buffer + p);
}

void framebuffer_t::set_depth(int x, int y, float depth) {
    assert(x >= 0 && x < width && y >= 0 && y < height);
    int p = (height - y - 1) * width + x;
    depth_buffer[p] = depth;
}

void framebuffer_t::set_color(int x, int y, vec4 color) {
    assert(x >= 0 && x < width && y >= 0 && y < height);
    int p = (height - y - 1) * width + x;
    ((uint*)color_buffer)[p] = rgba2rgbapack(color);
}

const uchar *framebuffer_t::get_color_data() const {
    return color_buffer;
}

const float *framebuffer_t::get_color_depth() const {
    return depth_buffer;
}



namespace {
    const int max_num_of_v2fs = 20;
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
        
        struct bbox_t {
            float xl, xr;
            float yl, yr;
        };

        bbox_t calc_bbox(vec2 p[3]) {
            auto min3f = [](float a, float b, float c) {return std::min(a, std::min(b, c));};
            auto max3f = [](float a, float b, float c) {return std::max(a, std::max(b, c));};
            float xl = floor(min3f(p[0].x(), p[1].x(), p[2].x()));
            float yl = floor(min3f(p[0].y(), p[1].y(), p[2].y()));
            float xr = ceil (max3f(p[0].x(), p[1].x(), p[2].x()));
            float yr = ceil (max3f(p[0].y(), p[1].y(), p[2].y()));
            return bbox_t{xl, xr, yl, yr};
        }

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

        void clip_aganst_panels(v2f_t* v2fs[], int indexes[], int &num) {
            const static vec4 planes[6] {
                vec4(0, 0, 1, 1), // near
                vec4(0, 0, -1, 1), // far
                vec4(1, 0, 0, 1), // left
                vec4(-1, 0, 0, 1), // right
                vec4(0, 1, 0, 1), // top
                vec4(0, -1, 0, 1) // bottom
            };
            int cur = 0;
            std::vector<int> input, output;
            for(int i = 0; i < 3; i++) {
                input.push_back(cur++);
            }
            for(int i = 0; !input.empty() && i < 6; i++) {
                const vec4 &C = planes[i];
                int p = 0, s = input.size() - 1;

                for(; p < input.size(); s = p++) {
                    int pp = input[p], sp = input[s];
                    
                    float d1 = v2fs[pp]->position.dot(C);
                    float d2 = v2fs[sp]->position.dot(C);
                    int situation = ((d1 >= 0) | ((d2 >= 0) << 1));

                    if(situation == 0) {
                        // do nothing
                    }
                    else if(situation == 1) {
                        if(fabs(d1 - d2) > EPSILON) {
                            lerp_v2f(v2fs[pp], v2fs[sp], d1 / (d1 - d2), v2fs[cur]);
                            output.push_back(cur);
                            cur++;
                        }
                        output.push_back(pp);
                    }
                    else if(situation == 2) {
                        if(fabs(d1 - d2) > EPSILON) {
                            lerp_v2f(v2fs[pp], v2fs[sp], d1 / (d1 - d2), v2fs[cur]);
                            output.push_back(cur);
                            cur++;
                        }
                    }
                    else if(situation == 3) {
                        output.push_back(pp);
                    }
                }
                input.swap(output);
                output.clear();
            }
            num = 0;
            for(int i = 1; i + 1 < input.size(); i++) {
                indexes[num] = input[0];
                indexes[num + 1] = input[i];
                indexes[num + 2] = input[i + 1];
                num += 3;
            }
        }

        bool back_face(vec2 p[3]) {
            vec2 a = p[1] - p[0], b = p[2] - p[0];
            float det = a.x() * b.y() - b.x() * a.y();
            return det < 0.0f;
        }

        float calc_line_distance(const vec2& v1, const vec2& v2, const vec2& point) {
            return (double)point.x() * (v1.y() - v2.y()) + (double)point.y() * (v2.x() - v1.x()) + ((double)v1.x() * v2.y() - (double)v2.x() * v1.y());
        }

        void rasterize(framebuffer_t *framebuffer, const v2f_t* v2fs[3], shader_t* shader) {
            int width = framebuffer->get_width();
            int height = framebuffer->get_height();
            int sizeof_varyings = shader->get_sizeof_varyings();
            float tr_rw[3], tr_z[3];
            
            vec2 screen_point[3];
            mat4 viewport_mat = viewport(width, height);
            

            for(int i = 0; i < 3; i++) {
                tr_rw[i] = 1.0f / v2fs[i]->position.w();
                tr_z[i] = v2fs[i]->position.z() * tr_rw[i];
                vec4 p = viewport_mat.mul_vec4(v2fs[i]->position * tr_rw[i]);
                screen_point[i] = vec2(p.x(), p.y());
            }
            
            // 背面剔除
            if(back_face(screen_point)) return ;

            float pre_calc[3];
            pre_calc[0] = calc_line_distance(screen_point[1], screen_point[2], screen_point[0]);
            pre_calc[1] = calc_line_distance(screen_point[2], screen_point[0], screen_point[1]);
            pre_calc[2] = calc_line_distance(screen_point[0], screen_point[1], screen_point[2]);
            for(int i = 0; i < 3; i++) {
                if(fabs(pre_calc[i]) < EPSILON) 
                    return ;
            }
            
            bbox_t bbox = calc_bbox(screen_point);
            bbox.xl = std::max(bbox.xl, 0.0f);   
            bbox.yl = std::max(bbox.yl, 0.0f);   
            bbox.xr = std::min(bbox.xr, width - 1.0f);   
            bbox.yr = std::min(bbox.yr, height - 1.0f);   
            
            v2f_t v2f(sizeof_varyings);

            for(int i = bbox.yl; i <= bbox.yr; i++) {
                for(int j = bbox.xl; j <= bbox.xr; j++) {
                    // shade
                    vec2 p(j + 0.5f, i + 0.5f);
                    float alpha = calc_line_distance(screen_point[1], screen_point[2], p) / pre_calc[0];
                    float beta  = calc_line_distance(screen_point[2], screen_point[0], p) / pre_calc[1];
                    float gamma = calc_line_distance(screen_point[0], screen_point[1], p) / pre_calc[2];

                    if(alpha < 0.0f || beta < 0.0f || gamma < 0.0f) continue;
                    
                    float depth = alpha * tr_z[0] + beta * tr_z[1] + gamma * tr_z[2];
                    depth = (depth + 1.0f) * 0.5f;
                    
                    if(framebuffer->get_depth(j, i) < depth) continue;
                    
                    interpolation_v2f(v2fs[0], v2fs[1], v2fs[2], alpha * tr_rw[0], beta * tr_rw[1], gamma * tr_rw[2], &v2f);
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
    
    int sizeof_varyings = shader->get_sizeof_varyings();

    int indexes[3 * max_num_of_v2fs];
    v2f_t* v2fs[max_num_of_v2fs];
    for(int i = 0; i < max_num_of_v2fs; i++) {
        v2fs[i] = new v2f_t(sizeof_varyings);
    }

    int count = data->get_count();
    for(int i = 0; i < count; i += 3) {
        for(int j = 0; j < 3; j++) {
            int ind = i + j;
            vec4 position = shader->vertex_shader(data->at(ind), v2fs[j]->data);
            v2fs[j]->position = position;
        }
        int num;
        clip_aganst_panels(v2fs, indexes, num);
        const v2f_t* tr_v2fs[3];
        for(int i = 0; i < num; i += 3) {
            for(int j = 0; j < 3; j++) {
                tr_v2fs[j] = v2fs[indexes[i + j]];
            }
            rasterize(framebuffer, tr_v2fs, shader);
        }
    }

    for(int i = 0; i < max_num_of_v2fs; i++) {
        delete v2fs[i];
    }
}