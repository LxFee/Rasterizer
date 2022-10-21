#include <cstring>
#include <iostream>
#include <string>

#include "core/api.h"
#include "shaders/blin_shader.h"


using namespace std;

const int window_width = 640, window_height = 640;
static const float CLICK_DELAY = 0.25f;
vec3 CAMERA_POSITION(0.0, 0.0, 5.0);
vec3 CAMERA_TARGET(0, 0, 0);

typedef struct {
    /* orbit */
    int is_orbiting;
    vec2 orbit_pos;
    vec2 orbit_delta;
    /* pan */
    int is_panning;
    vec2 pan_pos;
    vec2 pan_delta;
    /* zoom */
    float dolly_delta;
    /* click */
    float press_time;
    float release_time;
    vec2 press_pos;
    vec2 release_pos;
    int single_click;
    int double_click;
    vec2 click_pos;
} record_t;

static vec2 get_pos_delta(vec2 old_pos, vec2 new_pos) {
    vec2 delta = new_pos - old_pos;
    return delta / (float)window_height;
}

static vec2 get_cursor_pos(window_t *window) {
    float xpos, ypos;
    input_query_cursor(window, &xpos, &ypos);
    return vec2(xpos, ypos);
}

static void button_callback(window_t *window, button_t button, int pressed) {
    record_t *record = (record_t *)window_get_userdata(window);
    vec2 cursor_pos = get_cursor_pos(window);
    if(button == BUTTON_L) {
        float curr_time = platform_get_time();
        if(pressed) {
            record->is_orbiting = 1;
            record->orbit_pos = cursor_pos;
            record->press_time = curr_time;
            record->press_pos = cursor_pos;
        } else {
            float prev_time = record->release_time;
            vec2 pos_delta = get_pos_delta(record->orbit_pos, cursor_pos);
            record->is_orbiting = 0;
            record->orbit_delta = record->orbit_delta + pos_delta;
            if(prev_time && curr_time - prev_time < CLICK_DELAY) {
                record->double_click = 1;
                record->release_time = 0;
            } else {
                record->release_time = curr_time;
                record->release_pos = cursor_pos;
            }
        }
    } else if(button == BUTTON_R) {
        if(pressed) {
            record->is_panning = 1;
            record->pan_pos = cursor_pos;
        } else {
            vec2 pos_delta = get_pos_delta(record->pan_pos, cursor_pos);
            record->is_panning = 0;
            record->pan_delta = record->pan_delta + pos_delta;
        }
    }
}

static void scroll_callback(window_t *window, float offset) {
    record_t *record = (record_t *)window_get_userdata(window);
    record->dolly_delta += offset;
}

static void update_camera(window_t *window, pinned_camera_t *camera,
                          record_t *record) {
    vec2 cursor_pos = get_cursor_pos(window);
    if(record->is_orbiting) {
        vec2 pos_delta = get_pos_delta(record->orbit_pos, cursor_pos);
        record->orbit_delta = record->orbit_delta + pos_delta;
        record->orbit_pos = cursor_pos;
    }
    if(record->is_panning) {
        vec2 pos_delta = get_pos_delta(record->pan_pos, cursor_pos);
        record->pan_delta = record->pan_delta + pos_delta;
        record->pan_pos = cursor_pos;
    }
    if(input_key_pressed(window, KEY_SPACE)) {
        camera->set_transform(CAMERA_POSITION, CAMERA_TARGET);
    } else {
        motion_t motion;
        motion.orbit = record->orbit_delta;
        motion.pan = record->pan_delta;
        motion.dolly = record->dolly_delta;
        camera->update_transform(motion);
    }
}

void clear_record(record_t *record) {
    record->orbit_delta = vec2(0, 0);
    record->pan_delta = vec2(0, 0);
    record->dolly_delta = 0;
    record->single_click = 0;
    record->double_click = 0;
}

/* gui setup */
vec4 background(0.0f);
vec3 wall_rotation;
vec3 light_pos(1.0f);
pinned_camera_t camera(1.0 * window_width / window_height, PROJECTION_MODE_PERSPECTIVE);

void gui(window_t* window) {
    if(!window) return;
    ImGuiContext* ctx = (ImGuiContext*)window_get_gui_context(window);
    if(!ctx) return;
    int id = 0;
    ImGui::SetCurrentContext(ctx);
    ImGui::Begin("Info");
    ImGui::SliderFloat3("Light positon", light_pos.data(), -5, 5);
    ImGui::SliderFloat3("Wall rotation", wall_rotation.data(), -180, 180);
    auto eye = camera.get_position();
    auto at = camera.get_target();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}



int main(int argc, char *argv[]) {
    /* platform setup */
    platform_initialize();
    /* window & input setup */
    window_t *window = window_create("main window!", window_width, window_height);
    record_t record;
    memset(&record, 0, sizeof(record_t));
    window_set_userdata(window, &record);
    callbacks_t callbacks;
    callbacks.button_callback = button_callback;
    callbacks.scroll_callback = scroll_callback;
    callbacks.key_callback = NULL;
    input_set_callbacks(window, callbacks);

    /* mesh setup */
    mesh_t wall("assets/model/brickwall/brickwall.obj");

    /* texture setup */
    texture_t t_diffuse("assets/model/brickwall/brickwall_diffuse.jpg", USAGE_SRGB_COLOR);
    texture_t t_normal("assets/model/brickwall/brickwall_normal.jpg", USAGE_RAW_DATA);
    t_normal.set_interp_mode(SAMPLE_INTERP_MODE_NEAREST);

    /* camera setup */
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
    blin_uniforms.model_matrix = wall.get_model_matrix();
    blin_uniforms.camera_pos = camera.get_position();
    blin_uniforms.proj_matrix = camera.get_projection_matrix();
    blin_uniforms.view_matrix = camera.get_view_matrix();

    /* render */
    framebuffer_t framebuffer(window_width, window_height);
    while(!window_should_close(window)) {
        framebuffer.clear_color(background);
        framebuffer.clear_depth(1.0f);

        update_camera(window, &camera, &record);
        wall.set_rotation(wall_rotation);
        blin_uniforms.camera_pos = camera.get_position();
        blin_uniforms.proj_matrix = camera.get_projection_matrix();
        blin_uniforms.view_matrix = camera.get_view_matrix();
        point_lights[0].position = light_pos;
        for(int i = 0; i < 1; i++) {
            if(i == 0) wall.set_position(vec3(0, 0, 0));
            else wall.set_position(vec3(0, 0, 1 + (i - 1) * 2));
            blin_uniforms.model_matrix = wall.get_model_matrix();
            draw_primitives(&framebuffer, wall.get_vbo(), &blin_shader);
        }
        gui(window);
        window_draw_buffer(window, &framebuffer);
        clear_record(&record);
        input_poll_events();
    }

    platform_terminate();
}
