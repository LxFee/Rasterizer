#ifndef BLINSHADER_H_
#define BLINSHADER_H_

#include "core/api.h"

struct blin_point_light_t {
    vec3 position;
    vec3 color;
};

struct blin_varying_t {
    vec3 world_pos;
    vec3 world_normal;
    vec3 tangent;
    vec2 texcoords;
};

struct blin_uniform_t {
    /* common */
    mat4 model_matrix;
    mat4 view_matrix;
    mat4 proj_matrix;
    vec3 camera_pos;

    /* matierial */
    texture_t* normal_texture;
    texture_t* diffuse_texture;

    /* lights */
    int num_of_point_lights;
    blin_point_light_t* point_lights;
};

class blin_shader_t : public shader_t {
   public:
    blin_shader_t();
    const vec4 vertex_shader(const void* attribs, void* varyings) override;
    const vec4 fragment_shader(const void* varyings, bool& discard) override;
};

#endif  // BLINSHADER_H_