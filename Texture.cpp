#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION

#include "ext/stb_image.h"


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

vec4 Texture::sample(float u, float v) {
    if(u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f) {
        if()
    }
}

Texture* Texture::readfromfile(std::string image) {
    int width, height, nrChannels;
    unsigned char *data = stbi_load(image.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        if(nrChannels != 3 && nrChannels != 4) return NULL;
        Texture* texture = new Texture(width, height);
        if(nrChannels == 3) {
            for(int i = 0; i < width * height; i++) {
                texture->data[i] = unpackRGBA888(data + 3 * i);
            }
        } else { // nrChannels == 4
            for(int i = 0; i < width * height; i++) {
                texture->data[i] = unpackRGBA8888(data + 4 * i);
            }
        }
        stbi_image_free(data);
        return texture;
    }
    return NULL;
}