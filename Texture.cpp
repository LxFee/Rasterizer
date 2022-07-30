#include "Texture.h"

Texture::Texture(int w, int h) : w(w), h(h) {
    data.resize(w * h);
}


void Texture::query(int* w, int *h, int *surr, int *intp) {
    if(w) *w = this->w;
    if(h) *h = this->h;
    if(surr) *surr = this->surround;
    if(intp) *intp = this->interpolation;
}

void Texture::set_filled_color(vec4 color) {
    filled_color = color;
}

void Texture::set_srround(SURROUND surr) {
    surround = surr;
}

void Texture::set_interpolation(INTERPOLATION intp) {
    interpolation = intp;
}

Texture Texture::from_image(std::string image) {
    
}