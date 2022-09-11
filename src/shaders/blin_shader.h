#ifndef BLINSHADER_H_
#define BLINSHADER_H_

#include "api.h"

struct blin_varying_t {
    vec3 world_pos;
    vec3 world_normal;
    vec2 texcoords;
    mat3 tbn_matrix;
};

struct blin_uniform_t {
    mat4 model_matrix;
    mat4 view_matrix;
    mat4 proj_matrix;
    vec3 camera_pos;
    texture_t* normal_texture;
    texture_t* diffuse_texture;
};

class blin_shader_t : public shader_t {
public:
    blin_shader_t();
    const vec4 vertex_shader(const void *attribs, void *varyings) override ;
    const vec4 fragment_shader(const void *varyings, bool& discard) override ;
};

#endif // BLINSHADER_H_