#ifndef RASTERIZER_TEXTURE_H_
#define RASTERIZER_TEXTURE_H_

#include <string>
#include "maths.h"
#include "graphics.h"

typedef enum {
    USAGE_LDR_COLOR,
    USAGE_LDR_DATA,
    USAGE_HDR_COLOR,
    USAGE_HDR_DATA
} usage_t;


typedef enum {
    SAMPLE_INTERP_MODE_NEAREST,
    SAMPLE_INTERP_MODE_BILINEAR
} sample_interp_mode_t;

typedef enum {
    SAMPLE_SURROUND_MODE_CLAMP,
    SAMPLE_SURROUND_MODE_REPEAT
} sample_surround_mode_t;

class texture_t {
public:
    texture_t(int w, int h);
    texture_t(const std::string& filename, usage_t usage);
    ~texture_t();

    void load_from_file(const std::string& filename, usage_t usage);
    void load_from_colorbuffer(framebuffer_t* framebuffer);
    void load_from_depthbuffer(framebuffer_t* framebuffer);
    void sample(vec2 uv, 
                sample_interp_mode_t interp_mode = SAMPLE_INTERP_MODE_BILINEAR, 
                sample_surround_mode_t surround_mode = SAMPLE_SURROUND_MODE_REPEAT);
private:
    int width, height;
    vec4 *buffer;
};

class cube_texture_t {
public:
    cube_texture_t( const std::string& positive_x, const std::string& negative_x,
                    const std::string& positive_y, const std::string& negative_y,
                    const std::string& positive_z, const std::string& negative_z,
                    usage_t usage);
    ~cube_texture_t();

    void sample(vec3 direction, 
                sample_interp_mode_t interp_mode = SAMPLE_INTERP_MODE_BILINEAR, 
                sample_surround_mode_t surround_mode = SAMPLE_SURROUND_MODE_REPEAT);
private:
    texture_t* faces[6];
};

#endif // RASTERIZER_TEXTURE_H_