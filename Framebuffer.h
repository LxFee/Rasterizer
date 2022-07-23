#pragma once
#include "mathbase.h"
#include "Texture.h"
#include <vector>

class Framebuffer {
public:
    Framebuffer();
    void attach(Texture* texture);
    friend void mgl_update(Framebuffer* fb);
    friend void mgl_clear(vec4 col, Framebuffer* fb);
    friend void mgl_draw(int vbo, int count, Framebuffer* fb);
private:
    void clear(vec4 col);
    void set_pixel(int x, int y, vec4 col);
    void update();
    Texture* texture;
    std::vector<vec4> data;
    std::vector<float> zbuffer;
    int w, h;
};

