#pragma once
#include "mathbase.h"
#include "Texture.h"
#include <vector>

class Framebuffer {
public:
    Framebuffer();
    void attach(Texture* texture);
    void clear(vec4 col);
    friend void mgl_update(Framebuffer* fb);
    friend void mgl_draw(Framebuffer* fb, int vbo);
private:
    void set_pixel(int x, int y, vec4 col);
    void update();
    Texture* texture;
    std::vector<vec4> data;
    int w, h;
};

