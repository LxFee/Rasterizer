
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#include "Texture.h"
#define DATA(X, Y) data[(Y) * w + (X)]

vec4 bilinear(vec4 i00, vec4 i10, vec4 i01, vec4 i11, float u, float v) {
    vec4 i0 = i00 + u * (i10 - i00);
    vec4 i1 = i01 + u * (i11 - i01);
    return i0 + v * (i1 - i0);
}

Texture::Texture(int w, int h) : w(w), h(h) {
    data.resize(w * h);
}


void Texture::query(int* w, int *h, int *surr, int *intp) {
    if(w) *w = this->w;
    if(h) *h = this->h;
    if(surr) *surr = this->surround;
    if(intp) *intp = this->interpolation;
}

vec4 Texture::sample(float u, float v) {
    if(u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f) {
        if(surround == FILLED) {
            return filled_color;
        }
        if(surround == REPEAT) {
            u = u - floor(u);
            v = v - floor(v);
        }
    }
    if(interpolation == NEAREST) {
        int x = std::min(int(u * w), w - 1);
        int y = std::min(int(v * h), h - 1);
        return DATA(x, y);
    }
    if(interpolation == BILINEAR) {
        int x = u * (w - 1);
        int y = v * (h - 1);
        if(x == w - 1 || y == h - 1) return DATA(x, y);
        float local_u = u * (w - 1) - x; 
        float local_v = v * (h - 1) - y;
        return bilinear(DATA(x, y), DATA(x + 1, y), DATA(x, y + 1), DATA(x + 1, y + 1), local_u, local_v);
    }
    return vec4(0.0f);    
}

Texture* Texture::readfromfile(std::string image) {
    int width, height, nrChannels;
    unsigned char *data = stbi_load(image.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        if(nrChannels != 3 && nrChannels != 4) return NULL;
        Texture* texture = new Texture(width, height);
        if(nrChannels == 3) {
            for(int i = 0; i < height; i++) {
                for(int j = 0; j < width; j++) {
                    texture->data[(height - i - 1) * width + j] = unpackRGBA888(data + 3 * (i * width + j));
                }
            }
        } else { // nrChannels == 4
            for(int i = 0; i < height; i++) {
                for(int j = 0; j < width; j++) {
                    texture->data[(height - i - 1) * width + j] = unpackRGBA8888(data + 4 * (i * width + j));
                }
            }
        }
        stbi_image_free(data);
        return texture;
    }
    return NULL;
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