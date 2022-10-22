#include "core/image.h"

#include <cassert>
#include <cstring>

namespace {
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

bool stb_load_image(const std::string &filename, int &width, int &height,
                    int &channels, uchar **ldr_buffer) {
    uchar *data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    if(data == NULL) return false;
    int total_size = width * height * channels;
    *ldr_buffer = new uchar[total_size];
    memcpy(*ldr_buffer, data, total_size);
    stbi_image_free(data);
    return true;
}

const std::string get_ext_name(const std::string &filename) {
    std::string ext;
    if(filename.empty()) return ext;
    int p = filename.size() - 1;
    while(p > 0 && filename[p] != '.') p--;
    for(int i = p + 1; i < filename.size(); i++) {
        ext.push_back(filename[i]);
    }
    return ext;
}

}  // namespace

image_t::image_t(int _width, int _height, int _channels, format_t _format)
    : width(_width),
      height(_height),
      channels(_channels),
      format(_format),
      ldr_buffer(NULL),
      hdr_buffer(NULL),
      succeed(true) {
    int total_size = width * height * channels;
    switch(format) {
        case FORMAT_LDR:
            ldr_buffer = new uchar[total_size];
            break;
        case FORMAT_HDR:
            hdr_buffer = new float[total_size];
            break;
        default:
            succeed = false;
    }
}

image_t::image_t(const std::string &filename)
    : width(0),
      height(0),
      channels(0),
      format(),
      ldr_buffer(NULL),
      hdr_buffer(NULL),
      succeed(true) {
    std::string ext = get_ext_name(filename);
    if(ext == "png" || ext == "jpg") {
        format = FORMAT_LDR;
        succeed = stb_load_image(filename, width, height, channels, &ldr_buffer);
    }
}

image_t::~image_t() {
    if(ldr_buffer) delete[] ldr_buffer;
    if(hdr_buffer) delete[] hdr_buffer;
}

void image_t::flip_h() {
    if(format == FORMAT_LDR) {
        for(int i = 0; i < height / 2; i++) {
            for(int j = 0; j < width; j++) {
                uchar tmp;
                int p1 = (i * width + j) * channels,
                    p2 = ((height - i - 1) * width + j) * channels;
                for(int k = 0; k < channels; k++) {
                    tmp = ldr_buffer[p1 + k];
                    ldr_buffer[p1 + k] = ldr_buffer[p2 + k];
                    ldr_buffer[p2 + k] = tmp;
                }
            }
        }
    } else if(format == FORMAT_HDR) {
        for(int i = 0; i < height / 2; i++) {
            for(int j = 0; j < width; j++) {
                float tmp;
                int p1 = (i * width + j) * channels,
                    p2 = ((height - i - 1) * width + j) * channels;
                for(int k = 0; k < channels; k++) {
                    tmp = hdr_buffer[p1 + k];
                    hdr_buffer[p1 + k] = hdr_buffer[p2 + k];
                    hdr_buffer[p2 + k] = tmp;
                }
            }
        }
    }
}

void image_t::flip_v() {
    if(format == FORMAT_LDR) {
        for(int i = 0; i < height; i++) {
            for(int j = 0; j < width / 2; j++) {
                uchar tmp;
                int p1 = (i * width + j) * channels,
                    p2 = (i * width + width - j - 1) * channels;
                for(int k = 0; k < channels; k++) {
                    tmp = ldr_buffer[p1 + k];
                    ldr_buffer[p1 + k] = ldr_buffer[p2 + k];
                    ldr_buffer[p2 + k] = tmp;
                }
            }
        }
    } else if(format == FORMAT_HDR) {
        for(int i = 0; i < height; i++) {
            for(int j = 0; j < width / 2; j++) {
                float tmp;
                int p1 = (i * width + j) * channels,
                    p2 = (i * width + width - j - 1) * channels;
                for(int k = 0; k < channels; k++) {
                    tmp = hdr_buffer[p1 + k];
                    hdr_buffer[p1 + k] = hdr_buffer[p2 + k];
                    hdr_buffer[p2 + k] = tmp;
                }
            }
        }
    }
}

bool image_t::is_succeed() const { return succeed; }

int image_t::get_width() const { return width; }
int image_t::get_height() const { return height; }

int image_t::get_channels() const { return channels; }

format_t image_t::get_format() const { return format; }

const void *image_t::data() const {
    switch(format) {
        case FORMAT_LDR:
            return ldr_buffer;
        case FORMAT_HDR:
            return hdr_buffer;
    }
    return NULL;
}

void *image_t::data() {
    switch(format) {
        case FORMAT_LDR:
            return ldr_buffer;
        case FORMAT_HDR:
            return hdr_buffer;
    }
    return NULL;
}