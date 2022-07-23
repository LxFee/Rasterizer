#pragma once
#include "mathbase.h"
#include "Texture.h"
#include <vector>

class Framebuffer {
public:
    Framebuffer();
    void attach(Texture* texture);
    void set_pixel(int x, int y, vec4 col);
    void clear(vec4 col);
private:
    Texture* texture;
    std::vector<vec4> data;
    int w, h;
};

