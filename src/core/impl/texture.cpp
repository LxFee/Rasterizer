#include "texture.h"

#include <cassert>
#include <iostream>

#include "maths.h"

namespace {
vec4 bilinear(vec4 i00, vec4 i10, vec4 i01, vec4 i11, float u, float v) {
    vec4 i0 = i00 + (i10 - i00) * u;
    vec4 i1 = i01 + (i11 - i01) * u;
    return i0 + (i1 - i0) * v;
}
}  // namespace

void texture_t::ldr_image_to_texture(image_t* image) {
    uchar* image_data = (uchar*)image->data();
    int channels = image->get_channels();
    if(channels == 3) {
        for(int i = 0; i < height; i++) {
            for(int j = 0; j < width; j++) {
                buffer[j + i * width] =
                    rgbpack2rgba(image_data + (j + i * width) * 3);
            }
        }
    } else if(channels == 4) {
        for(int i = 0; i < height; i++) {
            for(int j = 0; j < width; j++) {
                buffer[j + i * width] =
                    rgbapack2rgba(image_data + (j + i * width) * 4);
            }
        }
    }
}

void texture_t::hdr_image_to_texture(image_t* image) {
    float* image_data = (float*)image->data();
    int channels = image->get_channels();
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            for(int k = 0; k < channels; k++) {
                buffer[j + i * width].data()[k] =
                    image_data[(j + i * width) * channels + k];
            }
        }
    }
}

void texture_t::srgb_to_linear() {
    for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
            for(int k = 0; k < 4; k++) {
                buffer[j + i * width].data()[k] =
                    float_linear2srgb(buffer[j + i * width].data()[k]);
            }
        }
    }
}

void texture_t::linear_to_srgb() {
    for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
            for(int k = 0; k < 4; k++) {
                buffer[j + i * width].data()[k] =
                    float_srgb2linear(buffer[j + i * width].data()[k]);
            }
        }
    }
}

texture_t::texture_t(int w, int h)
    : width(w), height(h), buffer(NULL), border_color(0.0f) {
    int total = width * height;
    buffer = new vec4[total];
}

texture_t::texture_t(const std::string& filename, usage_t usage)
    : width(), height(), buffer(NULL), border_color(0.0f) {
    image_t image(filename);
    width = image.get_width();
    height = image.get_height();
    buffer = new vec4[height * width];
    load_from_image(&image, usage);
}

texture_t::~texture_t() {
    if(buffer) delete[] buffer;
}

void texture_t::set_interp_mode(sample_interp_mode_t _interp_mode) {
    interp_mode = _interp_mode;
}

void texture_t::set_surround_mode(sample_surround_mode_t _surround_mode) {
    surround_mode = _surround_mode;
}

void texture_t::set_border_color(vec4 _color) { border_color = _color; }

void texture_t::load_from_file(const std::string& filename, usage_t usage) {
    image_t image(filename);
    assert(width == image.get_width() && height == image.get_width());
    load_from_image(&image, usage);
}

void texture_t::load_from_image(image_t* image, usage_t usage) {
    assert(image && image->is_succeed() && width == image->get_width() &&
           height == image->get_height());
    image->flip_h();
    if(image->get_format() == FORMAT_LDR) {
        ldr_image_to_texture(image);
        if(usage == USAGE_LINEAR_COLOR) {
            srgb_to_linear();
        }
    } else {
        hdr_image_to_texture(image);
        if(usage == USAGE_SRGB_COLOR) {
            linear_to_srgb();
        }
    }
}

void texture_t::load_from_colorbuffer(framebuffer_t* framebuffer) {
    assert(framebuffer && width == framebuffer->get_height() &&
           height == framebuffer->get_width());
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            buffer[i * width + j] = framebuffer->get_color(j, i);
        }
    }
}

void texture_t::load_from_depthbuffer(framebuffer_t* framebuffer) {
    assert(framebuffer && width == framebuffer->get_height() &&
           height == framebuffer->get_width());
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            buffer[i * width + j] = vec4(framebuffer->get_depth(j, i));
        }
    }
}

vec4 texture_t::sample(vec2 uv) {
    float u = uv.u(), v = uv.v();
    if(surround_mode == SAMPLE_SURROUND_MODE_BORDER) {
        if(u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f) return border_color;
    } else if(surround_mode == SAMPLE_SURROUND_MODE_CLAMP) {
        u = clamp(u, 0.0f, 1.0f);
        v = clamp(v, 0.0f, 1.0f);
    } else if(surround_mode == SAMPLE_SURROUND_MODE_REPEAT) {
        u = u - floor(u);
        v = v - floor(v);
    }
    if(interp_mode == SAMPLE_INTERP_MODE_NEAREST) {
        int x = std::min(int(u * width), width - 1);
        int y = std::min(int(v * height), height - 1);
        return buffer[y * width + x];
    } else if(interp_mode == SAMPLE_INTERP_MODE_BILINEAR) {
        int x = u * (width - 1);
        int y = v * (height - 1);
        if(x == width - 1 || y == height - 1) return buffer[y * width + x];
        float local_u = u * (width - 1) - x;
        float local_v = v * (height - 1) - y;
        return bilinear(buffer[y * width + x], buffer[y * width + x + 1],
                        buffer[y * width + x + width],
                        buffer[y * width + x + width + 1], local_u, local_v);
    }
    return vec4(0.0f);
}