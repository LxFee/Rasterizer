#include <cstring>
#include <iostream>
#include <string>
#include "utils/EventManager.h"

#include "core/api.h"
#include "shaders/blin_shader.h"


using namespace std;

const int window_width = 640, window_height = 640;
vec3 CAMERA_POSITION(0.0, 0.0, 5.0);
vec3 CAMERA_TARGET(0, 0, 0);

void register_input(window_t *window);
void gui(window_t* window);

/* gui setup */
vec4 background(0.0f);
vec3 wall_rotation;
vec3 light_pos(1.0f);

int main(int argc, char *argv[]) {
    /* platform setup */
    platform_initialize();
    /* window & input setup */
    window_t *window = window_create("main window!", window_width, window_height);
    register_input(window);

    /* mesh setup */
    mesh_t wall("assets/model/brickwall/brickwall.obj");

    /* texture setup */
    texture_t t_diffuse("assets/model/brickwall/brickwall_diffuse.jpg", USAGE_SRGB_COLOR);
    texture_t t_normal("assets/model/brickwall/brickwall_normal.jpg", USAGE_RAW_DATA);
    t_normal.set_interp_mode(SAMPLE_INTERP_MODE_NEAREST);

    /* camera setup */
    pinned_camera_t camera(1.0 * window_width / window_height, PROJECTION_MODE_PERSPECTIVE);
    camera.set_transform(CAMERA_POSITION, CAMERA_TARGET);

    /* lights */
    blin_point_light_t point_lights[1];
    point_lights[0].color = vec3(1.0f);
    point_lights[0].position = light_pos;

    /* shader setup */
    blin_uniform_t blin_uniforms;
    blin_shader_t blin_shader;
    blin_shader.bind_uniform(&blin_uniforms);

    /* uniform */
    memset(&blin_uniforms, 0, sizeof(blin_uniform_t));
    blin_uniforms.camera_pos = camera.get_position();
    blin_uniforms.diffuse_texture = &t_diffuse;
    blin_uniforms.normal_texture = &t_normal;
    blin_uniforms.num_of_point_lights = 1;
    blin_uniforms.point_lights = point_lights;
    blin_uniforms.model_matrix = euler_YXZ_rotate(wall_rotation);
    blin_uniforms.camera_pos = camera.get_position();
    blin_uniforms.proj_matrix = camera.get_projection_matrix();
    blin_uniforms.view_matrix = camera.get_view_matrix();

    /* render */
    framebuffer_t framebuffer(window_width, window_height);
    while(!window_should_close(window)) {
        framebuffer.clear_color(background);
        framebuffer.clear_depth(1.0f);

        camera.update_transform(window);

        blin_uniforms.camera_pos = camera.get_position();
        blin_uniforms.proj_matrix = camera.get_projection_matrix();
        blin_uniforms.view_matrix = camera.get_view_matrix();
        blin_uniforms.model_matrix = euler_YXZ_rotate(wall_rotation);
        point_lights[0].position = light_pos;
        draw_primitives(&framebuffer, wall.get_vbo(), &blin_shader);
        gui(window);
        window_draw_buffer(window, &framebuffer);
        input_poll_events();
    }

    platform_terminate();
    return 0;
}


void gui(window_t* window) {
    if(!window) return;
    ImGuiContext* ctx = (ImGuiContext*)window_get_gui_context(window);
    if(!ctx) return;
    int id = 0;
    ImGui::SetCurrentContext(ctx);
    ImGui::Begin("Info");
    ImGui::SliderFloat3("Light positon", light_pos.data(), -5, 5);
    ImGui::SliderFloat3("Wall rotation", wall_rotation.data(), -180, 180);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

void register_input(window_t *window) {
    pinned_camera_t::register_input();
    EventManager::registerEvent(SDLK_ESCAPE | Events::KEYBOARD_PRESS, [](window_t* window){
        window_close(window);
    });
}