#include "blin_shader.h"
#include <vector>
#include <iostream>

blin_shader_t::blin_shader_t()
    : shader_t(sizeof(blin_uniform_t), sizeof(blin_uniform_t)) {}


const vec4 blin_shader_t::vertex_shader(const void *attribs, void *varyings) {
    vertex_t *vertex = (vertex_t*)attribs;
    blin_varying_t *blin_varyings = (blin_varying_t *)varyings;
    blin_uniform_t *blin_uniforms = (blin_uniform_t *)uniforms;
    mat3 model = clip_mat4(blin_uniforms->model_matrix);
    mat4 mvp = blin_uniforms->proj_matrix 
             * blin_uniforms->view_matrix
             * blin_uniforms->model_matrix;

    vec3 T = model.mul_vec3(vertex->tangent).normalized();
    vec3 N = model.transpose().inverse().mul_vec3(vertex->normal).normalized();
    vec3 B = model.mul_vec3(vertex->bitangent).normalized();
    mat3 TBN(T, B, N);

    vec4 position(vertex->position, 1.0f);
    vec4 world_pos = blin_uniforms->model_matrix.mul_vec4(position);

    blin_varyings->tbn_matrix = TBN;
    blin_varyings->world_pos = vec3(world_pos.x(), world_pos.y(), world_pos.z());
    blin_varyings->world_normal = N;
    blin_varyings->texcoords = vertex->texcoord;
    return mvp.mul_vec4(position);
}


const vec4 blin_shader_t::fragment_shader(const void *varyings, bool& discard) {
    blin_varying_t *blin_varyings = (blin_varying_t *)varyings;
    const blin_uniform_t *blin_uniforms = (const blin_uniform_t *)uniforms;

    vec4 color(0.5f);
    vec3 normal = blin_varyings->world_normal;

    vec2 texcoords = blin_varyings->texcoords;
    if(blin_uniforms->diffuse_texture) {
        color = blin_uniforms->diffuse_texture->sample(texcoords);
    }

    return color;
    if(blin_uniforms->normal_texture) {
        vec4 t_normal = blin_uniforms->normal_texture->sample(texcoords);
        normal = blin_varyings->tbn_matrix.mul_vec3(vec3(t_normal.x(), t_normal.y(), t_normal.z()));
    }
    
    std::vector<vec3> light_pos = {vec3(3.0f, 2.0f, 4.0f)};
    std::vector<vec3> light_intensity = {vec3(40.0f, 40.0f, 40.0f)};

    vec3 ka = vec3(0.005, 0.005, 0.005);
    vec3 kd = vec3(color.x(), color.y(), color.z());
    vec3 ks = vec3(0.30, 0.30, 0.30);
    vec3 amb_light_intensity(10.0f, 10.0f, 10.0f);

    vec3 world_pos = blin_varyings->world_pos;
    vec3 camera_pos = blin_uniforms->camera_pos;

    vec3 view_dir = (camera_pos - world_pos).normalized();

    float p = 150.0f;
    vec3 result_color(0.0f);
    for(int i = 0; i < light_pos.size(); i++) {
        vec3 light_dir = light_pos[i] - world_pos;
        float r2 = light_dir.length_squared();
        light_dir = light_dir.normalized();
        vec3 h = (light_dir + view_dir).normalized();
        vec3 I = light_intensity[i] / r2;

        vec3 Ld = kd * I * std::max(0.0f, normal.dot(light_dir));
        vec3 Ls = ks * I * pow(std::max(0.0f, normal.dot(h)), p);
        vec3 La = ka * amb_light_intensity;

        result_color = result_color + Ld + Ls;
    }

    return vec4(result_color, 1.0f);
}