#include "core/graphics.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>


vbo_t::vbo_t(int _sizeof_element, int _count)
    : sizeof_element(_sizeof_element), count(_count), raw_data(NULL) {
    raw_data = new char[_sizeof_element * _count];
}

vbo_t::~vbo_t() { delete[] raw_data; }

void* vbo_t::data() { return raw_data; }

const void* vbo_t::data() const { return raw_data; }

void* vbo_t::at(int p) {
    assert(p >= 0 && p < count);
    return ((char*)raw_data) + p * sizeof_element;
}

const void* vbo_t::at(int p) const {
    assert(p >= 0 && p < count);
    return ((char*)raw_data) + p * sizeof_element;
}

int vbo_t::get_sizeof_element() const { return sizeof_element; }

int vbo_t::get_count() const { return count; }

int vbo_t::get_totol_size() const { return get_count() * get_sizeof_element(); }

framebuffer_t::framebuffer_t(int _width, int _height)
    : width(_width), height(_height), color_buffer(NULL), depth_buffer(NULL) {
    color_buffer = new uchar[width * height * 4];
    depth_buffer = new float[width * height];
}

framebuffer_t::~framebuffer_t() {
    delete[] color_buffer;
    delete[] depth_buffer;
}

int framebuffer_t::get_width() const { return width; }
int framebuffer_t::get_height() const { return height; }

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
    if(!(x >= 0 && x < width && y >= 0 && y < height)) {
        printf("%d %d\n", x, y);
    }
    assert(x >= 0 && x < width && y >= 0 && y < height);
    int p = (height - y - 1) * width + x;
    ((uint*)color_buffer)[p] = rgba2rgbapack(color);
}

const uchar* framebuffer_t::get_color_data() const { return color_buffer; }

const float* framebuffer_t::get_color_depth() const { return depth_buffer; }

namespace {
const int max_num_of_v2fs = 20;
namespace render {
struct v2f_t {
    v2f_t(int _sizeof_varing) {
        data = new char[_sizeof_varing];
        sizeof_varying = _sizeof_varing;
    }
    ~v2f_t() { delete[] data; }
    v2f_t(const v2f_t&) = delete;
    v2f_t& operator=(const v2f_t&) = delete;

    vec4 position;
    int sizeof_varying;
    char* data;
};

struct bbox_t {
    int xl, xr;
    int yl, yr;
};

void lerp_v2f(const v2f_t* a, const v2f_t* b, float t, v2f_t* target) {
    target->position = a->position + (b->position - a->position) * t;
    int count = (a->sizeof_varying) >> 2;
    float* data_a = (float*)a->data;
    float* data_b = (float*)b->data;
    float* data_c = (float*)target->data;
    for(int i = 0; i < count; i++) {
        data_c[i] = data_a[i] + (data_b[i] - data_a[i]) * t;
    }
}

void interpolation_v2f(const v2f_t* a, const v2f_t* b, const v2f_t* c, vec3 uvw, v2f_t* target) {
    float weight = 1.0 / (uvw.x() + uvw.y() + uvw.z());
    target->position = (a->position * uvw.x() + b->position * uvw.y() + c->position * uvw.z()) * weight;

    float* v  = (float*)target->data;
    float* va = (float*)a->data;
    float* vb = (float*)b->data;
    float* vc = (float*)c->data;
    int count = (a->sizeof_varying) >> 2;
    for(int i = 0; i < count; i++) {
        v[i] = (uvw.x() * va[i] + uvw.y() * vb[i] + uvw.z() * vc[i]) * weight;
    }
}

void clip_aganst_panels(v2f_t* v2fs[], int indexes[], int& num) {
    const static vec4 planes[6]{
        vec4(0, 0, 1, 1),   // near
        vec4(0, 0, -1, 1),  // far
        vec4(1, 0, 0, 1),   // left
        vec4(-1, 0, 0, 1),  // right
        vec4(0, 1, 0, 1),   // top
        vec4(0, -1, 0, 1)   // bottom
    };
    int cur = 0;
    std::vector<int> input, output;
    for(int i = 0; i < 3; i++) {
        input.push_back(cur++);
    }
    for(int i = 0; !input.empty() && i < 6; i++) {
        const vec4& C = planes[i];
        int p = 0, s = input.size() - 1;

        for(; p < input.size(); s = p++) {
            int pp = input[p], sp = input[s];

            float d1 = v2fs[pp]->position.dot(C);
            float d2 = v2fs[sp]->position.dot(C);
            int situation = ((d1 >= 0) | ((d2 >= 0) << 1));

            if(situation == 0) {
                // do nothing
            } else if(situation == 1) {
                if(fabs(d1 - d2) > EPSILON) {
                    lerp_v2f(v2fs[pp], v2fs[sp], d1 / (d1 - d2), v2fs[cur]);
                    output.push_back(cur);
                    cur++;
                }
                output.push_back(pp);
            } else if(situation == 2) {
                if(fabs(d1 - d2) > EPSILON) {
                    lerp_v2f(v2fs[pp], v2fs[sp], d1 / (d1 - d2), v2fs[cur]);
                    output.push_back(cur);
                    cur++;
                }
            } else if(situation == 3) {
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

struct vec2i {
    vec2i() = default;    

    // 四舍五入
    // vec2i(const vec2& v) : x(v.x()), y(v.y()) {}
    vec2i(const vec2& v) : x(v.x() + 0.5f), y(v.y() + 0.5f) {}
    vec2i(float x, float y) : x(x + 0.5f), y(y + 0.5f) {}
    
    vec2i(int x, int y) : x(x), y(y) {}

    const vec2i operator-(const vec2i& rhs) const {
        return vec2i(x - rhs.x, y - rhs.y);
    }

    int x, y;
};

bbox_t calc_bbox(vec2i a, vec2i b, vec2i c) {
    auto min3i = [](int a, int b, int c) {
        return std::min(a, std::min(b, c));
    };
    auto max3i = [](int a, int b, int c) {
        return std::max(a, std::max(b, c));
    };
    int xl = min3i(a.x, b.x, c.x);
    int yl = min3i(a.y, b.y, c.y);
    int xr = max3i(a.x, b.x, c.x);
    int yr = max3i(a.y, b.y, c.y);
    return bbox_t{xl, xr, yl, yr};
}

int edge_function(const vec2i& a, const vec2i& b, const vec2i& c) {
    vec2i v1 = b - a, v2 = c - a;
    return v1.x * v2.y - v2.x * v1.y;
}


// https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage
void rasterize(framebuffer_t* framebuffer, const v2f_t* v2fs[3], shader_t* shader, PRIMITIVE_TYPE type, int ignore_edge = 0) {
    int width = framebuffer->get_width();
    int height = framebuffer->get_height();
    int sizeof_varyings = shader->get_sizeof_varyings();
    
    mat4 viewport_mat = viewport(width, height);

    float one_div_w[3];
    vec4 p[3];
    vec2i v[3];

    for(int i = 0; i < 3; i++) {
        one_div_w[i] = 1.0f / v2fs[i]->position.w();
        p[i] = viewport_mat.mul_vec4(v2fs[i]->position * one_div_w[i]);
        v[i] = vec2i(p[i].x(), p[i].y());
    }
    
    vec2i edge0 = v[2] - v[1]; 
    vec2i edge1 = v[0] - v[2]; 
    vec2i edge2 = v[1] - v[0]; 


    int area = edge_function(v[0], v[1], v[2]);
    if(area == 0) return ;

    // 背面剔除
    int backface = sgn(area);
    if(backface < 0) return ;

    bbox_t bbox = calc_bbox(v[0], v[1], v[2]);
    bbox.xl = std::max(bbox.xl, 0);
    bbox.yl = std::max(bbox.yl, 0);
    bbox.xr = std::min(bbox.xr, width - 1);
    bbox.yr = std::min(bbox.yr, height - 1);

    v2f_t v2f(sizeof_varyings);

    auto shade = [&](int x, int y) {
        vec2i cur_p(x, y);
        // If the point is on the edge, test if it is a top or left edge, 
        // otherwise test if  the edge function is ok
        int da = edge_function(v[1], v[2], cur_p);
        int db = edge_function(v[2], v[0], cur_p);
        int dc = edge_function(v[0], v[1], cur_p);

        float alpha = 1.0f * da / area;
        float beta  = 1.0f * db / area;
        float gamma = 1.0f * dc / area;

        float z = alpha * p[0].z() + beta * p[1].z() + gamma * p[2].z();
        float depth = (z + 1.0f) * 0.5f;

        // 深度测试 - early Z
        if(framebuffer->get_depth(x, y) < depth) return ;

        // 重心坐标插值+透视矫正
        vec3 uvw(alpha * one_div_w[0], beta * one_div_w[1], gamma * one_div_w[2]);
        interpolation_v2f(v2fs[0], v2fs[1], v2fs[2], uvw, &v2f);

        // fragment shader
        bool discord = false;
        vec4 color = shader->fragment_shader(v2f.data, discord);
        if(discord) return ;

        // update buffer
        framebuffer->set_depth(x, y, depth);
        framebuffer->set_color(x, y, color);
    };

    auto rasterize_filled_triangle = [&]() {
        for(int i = bbox.yl; i <= bbox.yr; i++) {
            for(int j = bbox.xl; j <= bbox.xr; j++) {
                // shade
                bool overlaps = true;
                vec2i P(j, i);
                int da = edge_function(v[1], v[2], P);
                int db = edge_function(v[2], v[0], P);
                int dc = edge_function(v[0], v[1], P);
                overlaps &= (da == 0 ? ((edge0.y == 0 && backface * edge0.x < 0) || backface * edge0.y < 0) : (backface * sgn(da) > 0)); 
                overlaps &= (db == 0 ? ((edge1.y == 0 && backface * edge1.x < 0) || backface * edge1.y < 0) : (backface * sgn(db) > 0)); 
                overlaps &= (dc == 0 ? ((edge2.y == 0 && backface * edge2.x < 0) || backface * edge2.y < 0) : (backface * sgn(dc) > 0)); 
                if(overlaps) shade(j, i);
            }
        }
    };

    auto rasterize_wire_frame_triangle = [&](int p1, int p2) {
        int x0 = v[p1].x, y0 = v[p1].y;
        int x1 = v[p2].x, y1 = v[p2].y;
        int dx = abs(x0 - x1), dy = abs(y0 - y1);
        if(dx < dy) {
            std::swap(x0, y0);
            std::swap(x1, y1);
        }
        if(x0 > x1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        if(dx < dy) {
            x1 = std::min(x1, height - 1);
        } else {
            x1 = std::min(x1, width - 1);
        }

        int A = y1 - y0;
        int B = x0 - x1;
        int C = -(A * x0 + B * y0);

        int sgny = y0 < y1 ? 1 : -1;

        int d = 2 * A * (x0 + 1) + B * (2 * y0 + sgny) + 2 * C;

        int y = y0;
        for(auto x = x0 ; x <= x1 ; ++x) {
            if(x >= 0 && y >= 0) {
                if(dx < dy) {
                    if(y < width)
                        shade(y, x);
                } else {
                    if(y < height)
                        shade(x, y);
                }
            }
            if(sgny * sgn(B) * sgn(d) < 0) {
                d += 2 * (A + sgny * B);
                y += sgny;
            } else d += 2 * A; 
        }
    };
    if(type == TRIANGLE) {
        rasterize_filled_triangle();
    }
    else if(type == TRIANGLE_WIRE_FRAME) {
        if(!(ignore_edge & 1)) rasterize_wire_frame_triangle(0, 1);
        if(!(ignore_edge & 2)) rasterize_wire_frame_triangle(1, 2);
        if(!(ignore_edge & 4)) rasterize_wire_frame_triangle(2, 0);
    }
    else {
        assert(0 && "Unknown primitive type!");
    }
}

}  // namespace render
}  // namespace

void draw_primitives(framebuffer_t* framebuffer, const vbo_t* data, shader_t* shader, PRIMITIVE_TYPE type) {
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
            if(num == 3) {
                rasterize(framebuffer, tr_v2fs, shader, type, 0);
            } else if(i == 0) {
                rasterize(framebuffer, tr_v2fs, shader, type, 4);
            } else if(i == num - 3) {
                rasterize(framebuffer, tr_v2fs, shader, type, 1);
            } else {
                rasterize(framebuffer, tr_v2fs, shader, type, 1 | 4);
            }
        }
    }

    for(int i = 0; i < max_num_of_v2fs; i++) {
        delete v2fs[i];
    }
}