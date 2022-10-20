#include "blin_shader.h"

#include <iostream>
#include <vector>

blin_shader_t::blin_shader_t() : shader_t(sizeof(blin_varying_t)) {}

const vec4 blin_shader_t::vertex_shader(const void *attribs, void *varyings) {
    vertex_t *vertex = (vertex_t *)attribs;
    blin_varying_t *blin_varyings = (blin_varying_t *)varyings;
    blin_uniform_t *blin_uniforms = (blin_uniform_t *)uniforms;
    
    mat3 model = clip_mat4(blin_uniforms->model_matrix);
    mat4 mvp = blin_uniforms->proj_matrix * blin_uniforms->view_matrix * blin_uniforms->model_matrix;

    vec3 T = model.mul_vec3(vertex->tangent).normalized();
    vec3 N = model.transpose().inverse().mul_vec3(vertex->normal).normalized();

    vec4 position(vertex->position, 1.0f);
    vec4 world_pos = blin_uniforms->model_matrix.mul_vec4(position);

    blin_varyings->tangent = T;
    blin_varyings->world_pos = vec3(world_pos.x(), world_pos.y(), world_pos.z());
    blin_varyings->world_normal = N;
    blin_varyings->texcoords = vertex->texcoord;

    return mvp.mul_vec4(position);
}

const vec4 blin_shader_t::fragment_shader(const void *varyings, bool &discard) {
    blin_varying_t *blin_varyings = (blin_varying_t *)varyings;
    const blin_uniform_t *blin_uniforms = (const blin_uniform_t *)uniforms;

    vec3 normal = blin_varyings->world_normal.normalized();
    vec2 texcoords = blin_varyings->texcoords;

    assert(blin_uniforms->diffuse_texture);
    vec4 t_color = blin_uniforms->diffuse_texture->sample(texcoords);
    vec3 color(t_color.x(), t_color.y(), t_color.z());
    
    if(blin_uniforms->normal_texture) {
        vec4 t_normal = blin_uniforms->normal_texture->sample(texcoords);
        t_normal = t_normal * 2.0f - 1.0f;
        vec3 N = normal;
        vec3 T = blin_varyings->tangent.normalized();
        T = (T - N * T.dot(N)).normalized();
        vec3 B = cross(T, N);
        mat3 TBN(T, B, N);
        normal = TBN.mul_vec3(vec3(t_normal.x(), t_normal.y(), t_normal.z()).normalized()).normalized();
    }

    vec3 world_pos = blin_varyings->world_pos;
    vec3 camera_pos = blin_uniforms->camera_pos;

    vec3 view_dir = (camera_pos - world_pos).normalized();

    vec3 pixel_color(0.0f);

    int num_of_point_lights = blin_uniforms->num_of_point_lights;
    for(int i = 0; i < num_of_point_lights; i++) {
        blin_point_light_t &point_light = blin_uniforms->point_lights[i];
        vec3 light_pos = point_light.position;
        vec3 light_color = point_light.color;

        vec3 light_dir = light_pos - world_pos;
        float dis = light_dir.length() + 0.1;
        light_dir = light_dir.normalized();

        // ambient
        vec3 ambient = light_color * 0.1;

        // diffuse
        float diff = std::max(light_dir.dot(normal), 0.0f);
        vec3 diffuse = light_color * diff;

        // specular
        vec3 h = (light_dir + view_dir).normalized();
        float spec = pow(std::max(normal.dot(h), 0.0f), 64.0f);
        vec3 specular =  light_color * spec;

        pixel_color = pixel_color + color * (ambient + (specular + diffuse) / dis);
    }
    return vec4(pixel_color, 1.0f);
}