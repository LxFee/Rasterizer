#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION

#include "ext/stb_image.h"

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

void Texture::swap(std::vector<vec4> &rdata) {
    rdata.swap(data);
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
        int x = u * (w - 1);
        int y = v * (h - 1);
        return data[y * w + x];
    }
    if(interpolation == BILINEAR) {
        int x = u * (w - 1);
        int y = v * (h - 1);
        float local_u = u * (w - 1) - x; 
        float local_v = v * (h - 1) - y;
        bool fu = local_u < 0.5f, fv = local_v < 0.5f;
        if(fu && fv) { // 左下
            if(x == 0 || y == 0) return DATA(x, y);
            local_u += 0.5;
            local_v += 0.5;
            return bilinear(DATA(x - 1, y - 1), DATA(x, y - 1), DATA(x - 1, y), DATA(x, y), local_u, local_v);
        }
        else if(fu && !fv) { // 左上
            if(x == 0 || y == h - 1) return DATA(x, y); 
            local_u += 0.5;
            local_v -= 0.5;
            return bilinear(DATA(x - 1, y), DATA(x, y), DATA(x - 1, y + 1), DATA(x, y + 1), local_u, local_v);
        }
        else if(!fu && fv) { // 右下
            if(x == w - 1 || y == 0) return DATA(x, y);
            local_u -= 0.5;
            local_v += 0.5;
            return bilinear(DATA(x, y - 1), DATA(x + 1, y), DATA(x, y), DATA(x + 1, y), local_u, local_v);
        }
        else if(!fu && !fv) {// 右上
            if(x == w - 1 || y == h - 1) return DATA(x, y);
            local_u -= 0.5;
            local_v -= 0.5;
            return bilinear(DATA(x, y), DATA(x + 1, y), DATA(x, y + 1), DATA(x + 1, y + 1), local_u, local_v);
        }
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
