#include "blin_shader.h"

#include <iostream>
#include <vector>

namespace {
blin_material_t get_default_material() {
    static blin_material_t material;
    static bool first = true;
    if(first) {
        first = false;
        material.ambient = vec3(0.01f);
        material.diffuse = vec3(1.0f);
        material.specular = vec3(0.30f);
        material.shininess = 150.0f;
    }
    return material;
}
}  // namespace

vec3 blin_point_light_t::distance2attenuation(float distance) {
    static float attenuation[12][4] = {
        7,   1.0, 0.7,   1.8,    13,   1.0, 0.35,   0.44,
        20,  1.0, 0.22,  0.20,   32,   1.0, 0.14,   0.07,
        50,  1.0, 0.09,  0.032,  65,   1.0, 0.07,   0.017,
        100, 1.0, 0.045, 0.0075, 160,  1.0, 0.027,  0.0028,
        200, 1.0, 0.022, 0.0019, 325,  1.0, 0.014,  0.0007,
        600, 1.0, 0.007, 0.0002, 3250, 1.0, 0.0014, 0.000007};
    int i;
    for(i = 0; i < 11; i++) {
        if(distance <= attenuation[i][0]) break;
    }
    return vec3(attenuation[i][1], attenuation[i][2], attenuation[i][3]);
}

blin_shader_t::blin_shader_t() : shader_t(sizeof(blin_varying_t)) {}

const vec4 blin_shader_t::vertex_shader(const void *attribs, void *varyings) {
    vertex_t *vertex = (vertex_t *)attribs;
    blin_varying_t *blin_varyings = (blin_varying_t *)varyings;
    blin_uniform_t *blin_uniforms = (blin_uniform_t *)uniforms;
    mat3 model = clip_mat4(blin_uniforms->model_matrix);
    mat4 mvp = blin_uniforms->proj_matrix * blin_uniforms->view_matrix *
               blin_uniforms->model_matrix;

    vec3 T = model.mul_vec3(vertex->tangent).normalized();
    vec3 N = model.transpose().inverse().mul_vec3(vertex->normal).normalized();
    // vec3 B = model.mul_vec3(vertex->bitangent).normalized();
    T = (T - N * T.dot(N)).normalized();
    vec3 B = cross(T, N);
    mat3 TBN(T, B, N);

    vec4 position(vertex->position, 1.0f);
    vec4 world_pos = blin_uniforms->model_matrix.mul_vec4(position);

    blin_varyings->tbn_matrix = TBN;
    blin_varyings->world_pos =
        vec3(world_pos.x(), world_pos.y(), world_pos.z());
    blin_varyings->world_normal = N;
    blin_varyings->texcoords = vertex->texcoord;

    return mvp.mul_vec4(position);
}

const vec4 blin_shader_t::fragment_shader(const void *varyings, bool &discard) {
    blin_varying_t *blin_varyings = (blin_varying_t *)varyings;
    const blin_uniform_t *blin_uniforms = (const blin_uniform_t *)uniforms;

    blin_material_t material(get_default_material());
    if(blin_uniforms->blin_material) material = *(blin_uniforms->blin_material);

    vec3 normal = blin_varyings->world_normal.normalized();
    vec2 texcoords = blin_varyings->texcoords;

    if(blin_uniforms->diffuse_texture) {
        vec4 diffuse_color = blin_uniforms->diffuse_texture->sample(texcoords);
        material.diffuse =
            vec3(diffuse_color.x(), diffuse_color.y(), diffuse_color.z());
    }

    if(blin_uniforms->specular_texture) {
        vec4 specular_color =
            blin_uniforms->specular_texture->sample(texcoords);
        material.specular =
            vec3(specular_color.x(), specular_color.y(), specular_color.z());
    }

    if(blin_uniforms->normal_texture) {
        vec4 t_normal = blin_uniforms->normal_texture->sample(texcoords);
        t_normal = t_normal * 2.0f - 1.0f;
        normal =
            blin_varyings->tbn_matrix
                .mul_vec3(
                    vec3(t_normal.x(), t_normal.y(), t_normal.z()).normalized())
                .normalized();
    }

    vec3 world_pos = blin_varyings->world_pos;
    vec3 camera_pos = blin_uniforms->camera_pos;

    vec3 view_dir = (camera_pos - world_pos).normalized();

    vec3 pixel_color(0.0f);

    int num_of_point_lights = blin_uniforms->num_of_point_lights;
    for(int i = 0; i < num_of_point_lights; i++) {
        blin_point_light_t &point_light = blin_uniforms->point_lights[i];
        vec3 light_dir = point_light.position - world_pos;
        float attenuation =
            1.0f / point_light.attenuation.dot(vec3(
                       1.0f, light_dir.length(), light_dir.length_squared()));
        light_dir = light_dir.normalized();
        vec3 h = (light_dir + view_dir).normalized();
        vec3 I = point_light.intensity * attenuation;

        vec3 Ld = material.diffuse * I * std::max(0.0f, normal.dot(light_dir));
        vec3 Ls = material.specular * I *
                  pow(std::max(0.0f, normal.dot(h)), material.shininess);
        vec3 La = material.diffuse * material.ambient *
                  blin_uniforms->amb_light_intensity;

        pixel_color = pixel_color + Ls + Ld + La;
    }
    return vec4(float_linear2srgb(pixel_color.x()),
                float_linear2srgb(pixel_color.y()),
                float_linear2srgb(pixel_color.z()), 1.0f);
}