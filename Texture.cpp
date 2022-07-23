#include "Texture.h"

Texture::Texture(int w, int h) : w(w), h(h) {
    data.resize(w * h);
}


void Texture::get_size(int* w, int *h) {
    if(w) *w = this->w;
    if(h) *h = this->h;
}

void Texture::swap(std::vector<vec4> &rdata) {
    rdata.swap(data);
}