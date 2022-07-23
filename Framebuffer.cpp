#include "Framebuffer.h"
#include <cstddef>

Framebuffer::Framebuffer() {
    w = 0, h = 0;
    texture = NULL;
}

void Framebuffer::clear(vec4 col) {
    for(auto &d : data) {
        d = col;
    }
}

void Framebuffer::attach(Texture* texture) {
    this->texture = texture;
    if(texture) {
        int nw, nh;
        texture->get_size(&nw, &nh);
        this->w = nw;
        this->h = nh;
        data.resize(w * h);
    }
}


void Framebuffer::set_pixel(int x, int y, vec4 col) {
    int index = x + y * w;
    if(index >= h * w) return ;
    data[index] = col;
}

void Framebuffer::update() {
    if(texture) {
        texture->swap(data);
    }
}

