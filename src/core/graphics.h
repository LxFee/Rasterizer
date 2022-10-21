#ifndef RASTERIZER_GRAPHICS_H_
#define RASTERIZER_GRAPHICS_H_

#include "marco.h"
#include "maths.h"
#include "shader.h"

enum PRIMITIVE_TYPE {
    TRIANGLE, TRIANGLE_WIRE_FRAME
};

class vbo_t {
   public:
    vbo_t(int _sizeof_element, int _count);
    ~vbo_t();

    vbo_t(const vbo_t&) = delete;
    vbo_t& operator=(const vbo_t&) = delete;

    void* data();
    const void* data() const;

    void* at(int p);
    const void* at(int p) const;

    int get_sizeof_element() const;
    int get_count() const;
    int get_totol_size() const;

   private:
    int sizeof_element, count;
    char* raw_data;
};

class framebuffer_t {
   public:
    framebuffer_t(int _width, int _height);
    ~framebuffer_t();

    framebuffer_t(const framebuffer_t&) = delete;
    framebuffer_t& operator=(const framebuffer_t&) = delete;

    int get_width() const;
    int get_height() const;

    void clear_color(vec4 color);
    void clear_depth(float depth);

    float get_depth(int x, int y) const;
    const vec4 get_color(int x, int y) const;

    void set_depth(int x, int y, float depth);
    void set_color(int x, int y, vec4 color);

    const uchar* get_color_data() const;
    const float* get_color_depth() const;

   private:
    int width, height;
    uchar* color_buffer;
    float* depth_buffer;
};

void draw_primitives(framebuffer_t* framebuffer, const vbo_t* data, shader_t* shader, PRIMITIVE_TYPE type = TRIANGLE);

#endif  // RASTERIZER_GRAPHIC_H_