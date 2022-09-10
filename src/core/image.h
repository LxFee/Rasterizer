#ifndef RASTERIZER_IMAGE_H_
#define RASTERIZER_IMAGE_H_

#include <string>
#include "maths.h"

typedef enum {
    FORMAT_LDR,
    FORMAT_HDR
} format_t;

class image_t {
public:
    image_t(int width, int height, int channels, format_t format);
    image_t(const std::string& filename);
    ~image_t();

    void flip_h();
    void flip_v();
    void save(const std::string& filename);

private:
    format_t format;
    int width, height, channels;
    unsigned char *ldr_buffer;
    float *hdr_buffer;
};

#endif // RASTERIZER_IMAGE_H_
