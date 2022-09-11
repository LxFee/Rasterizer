#ifndef RASTERIZER_IMAGE_H_
#define RASTERIZER_IMAGE_H_

#include <string>
#include "maths.h"
#include "marco.h"

typedef enum {
    FORMAT_LDR,
    FORMAT_HDR
} format_t;

class image_t {
public:
    image_t(int _width, int _height, int _channels, format_t _format);
    image_t(const std::string& filename);
    ~image_t();

    bool is_succeed() const ;

    void flip_h();
    void flip_v();

    int get_width() const ;
    int get_height() const ;
    int get_channels() const ;
    const void *data() const ;
    void *data();

    format_t get_format() const ;

    image_t(const image_t&) = delete;
    image_t &operator = (const image_t&) = delete;

private:
    format_t format;
    int width, height, channels;
    uchar *ldr_buffer;
    float *hdr_buffer;
    bool succeed;
};

#endif // RASTERIZER_IMAGE_H_
