#include <cstring>
#include <iostream>
#include <string>

#include "core/api.h"
#include "shaders/blin_shader.h"
#include "utils/EventManager.h"

using namespace std;

const int w = 800, h = 600;
static const vec3 CAMERA_POSITION(0, 0, 15);
static const vec3 CAMERA_TARGET(0, 0, 0);
bool wire_frame;

void gui(window_t* window);
void register_input(window_t* window);

int main(int argc, char *argv[]) {
    /* platform setup */
    platform_initialize();

    /* window & input setup */
    window_t *window = window_create("main window!", w, h);
    register_input(window);

    /* mesh setup */
    mesh_t cow("assets/model/cow/cow.obj");
    mat4 cow_model;

    /* texture setup */
    texture_t t_diffuse("assets/model/cow/cow_diffuse.png", USAGE_SRGB_COLOR);

    /* camera setup */
    pinned_camera_t camera(800.0f / 600.0f, PROJECTION_MODE_PERSPECTIVE);
    camera.set_zoom(90.0f);
    camera.set_transform(CAMERA_POSITION, CAMERA_TARGET);

    /* lights */
    vec3 light_color1(1.0f), light_color2(1.0f);
    blin_point_light_t point_lights[2];
    point_lights[0].color = light_color1;
    point_lights[0].position = vec3(3.0f, 4.0f, -3.0f);

    point_lights[1].color = light_color2;
    point_lights[1].position = vec3(-3.0f, 4.0f, -3.0f);

    /* shader setup */
    blin_uniform_t blin_uniforms;
    blin_shader_t blin_shader;
    blin_shader.bind_uniform(&blin_uniforms);

    /* uniform */
    memset(&blin_uniforms, 0, sizeof(blin_uniform_t));
    blin_uniforms.camera_pos = camera.get_position();
    blin_uniforms.diffuse_texture = &t_diffuse;
    blin_uniforms.normal_texture = NULL;
    blin_uniforms.num_of_point_lights = 2;
    blin_uniforms.point_lights = point_lights;
    blin_uniforms.model_matrix = cow_model;
    blin_uniforms.camera_pos = camera.get_position();
    blin_uniforms.proj_matrix = camera.get_projection_matrix();
    blin_uniforms.view_matrix = camera.get_view_matrix();

    /* gui setup */
    vec4 background;
    vec3 cow_rotation;

    /* render */
    framebuffer_t framebuffer(w, h);
    while(!window_should_close(window)) {
        framebuffer.clear_color(background);
        framebuffer.clear_depth(1.0f);

        camera.update_transform(window);
        cow_model = euler_YXZ_rotate(cow_rotation) * scale(vec3(5.0f));
        
        // mvp
        blin_uniforms.model_matrix = cow_model;
        blin_uniforms.proj_matrix = camera.get_projection_matrix();
        blin_uniforms.view_matrix = camera.get_view_matrix();
        
        // camera position
        blin_uniforms.camera_pos = camera.get_position();
        
        // render
        if(wire_frame) {
            draw_primitives(&framebuffer, cow.get_vbo(), &blin_shader, TRIANGLE_WIRE_FRAME);
        } else {
            draw_primitives(&framebuffer, cow.get_vbo(), &blin_shader);
        }
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
    ImGui::Checkbox("Wire Frame", &wire_frame);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

void register_input(window_t* window) {
    pinned_camera_t::register_input();
    EventManager::registerEvent(SDLK_ESCAPE | Events::KEYBOARD_PRESS, [](window_t* window){
        window_close(window);
    });
}