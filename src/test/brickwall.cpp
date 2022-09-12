#include <iostream>
#include <string>
#include "api.h"
#include "blin_shader.h"
#include <cstring>

using namespace std;

const int w = 800, h = 600;
static const float CLICK_DELAY = 0.25f;
static const vec3 CAMERA_POSITION(0, 0, 15);
static const vec3 CAMERA_TARGET(0, 0, 0);

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
    return delta / (float)h;
}

static vec2 get_cursor_pos(window_t *window) {
    float xpos, ypos;
    input_query_cursor(window, &xpos, &ypos);
    return vec2(xpos, ypos);
}

static void button_callback(window_t *window, button_t button, int pressed) {
    record_t *record = (record_t*)window_get_userdata(window);
    vec2 cursor_pos = get_cursor_pos(window);
    if (button == BUTTON_L) {
        float curr_time = platform_get_time();
        if (pressed) {
            record->is_orbiting = 1;
            record->orbit_pos = cursor_pos;
            record->press_time = curr_time;
            record->press_pos = cursor_pos;
        } else {
            float prev_time = record->release_time;
            vec2 pos_delta = get_pos_delta(record->orbit_pos, cursor_pos);
            record->is_orbiting = 0;
            record->orbit_delta = record->orbit_delta + pos_delta;
            if (prev_time && curr_time - prev_time < CLICK_DELAY) {
                record->double_click = 1;
                record->release_time = 0;
            } else {
                record->release_time = curr_time;
                record->release_pos = cursor_pos;
            }
        }
    } else if (button == BUTTON_R) {
        if (pressed) {
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
    record_t *record = (record_t*)window_get_userdata(window);
    record->dolly_delta += offset;
}

static void clear_record(record_t *record) {
    memset(record, 0, sizeof(record_t));
}

static void update_camera(window_t *window, pinned_camera_t *camera,
                          record_t *record) {
    vec2 cursor_pos = get_cursor_pos(window);
    if (record->is_orbiting) {
        vec2 pos_delta = get_pos_delta(record->orbit_pos, cursor_pos);
        record->orbit_delta = record->orbit_delta + pos_delta;
        record->orbit_pos = cursor_pos;
    }
    if (record->is_panning) {
        vec2 pos_delta = get_pos_delta(record->pan_pos, cursor_pos);
        record->pan_delta = record->pan_delta + pos_delta;
        record->pan_pos = cursor_pos;
    }
    if (input_key_pressed(window, KEY_SPACE)) {
        camera->set_transform(CAMERA_POSITION, CAMERA_TARGET);
    } else {
        motion_t motion;
        motion.orbit = record->orbit_delta;
        motion.pan = record->pan_delta;
        motion.dolly = record->dolly_delta;
        camera->update_transform(motion);
    }
}



int main(int argc, char* argv[]) {
    platform_initialize();
    window_t *window = window_create("main window!", w, h);
    record_t record;
    memset(&record, 0, sizeof(record_t));
    window_set_userdata(window, &record);
    callbacks_t callbacks;
    callbacks.button_callback = button_callback;
    callbacks.scroll_callback = scroll_callback;
    callbacks.key_callback = NULL;
    input_set_callbacks(window, callbacks);
    
    framebuffer_t framebuffer(w, h);
    vec4 background;
    vec3 wall_rotation;
    widget_t demo_gui {
        "normal wall", 
        {
            {
                "background", 
                {
                    {"color", ITEM_TYPE_COLOR4, background.data()}
                }
            },
            {
                "wall", 
                {
                    {"rotation", ITEM_TYPE_FLOAT3, wall_rotation.data(), -180.0f, 180.0f}
                }
            },
        }
    };

    mesh_t wall("assets/model/brickwall/brickwall.obj");
    wall.set_size(vec3(8.0f));
    texture_t t_diffuse("assets/model/brickwall/brickwall_diffuse.jpg", USAGE_LDR_COLOR);
    texture_t t_normal("assets/model/brickwall/brickwall_normal.jpg", USAGE_LDR_COLOR);
    t_normal.set_interp_mode(SAMPLE_INTERP_MODE_NEAREST);

    pinned_camera_t camera(800.0f / 600.0f, PROJECTION_MODE_PERSPECTIVE);
    camera.set_near(0.1f);
    camera.set_far(1000.0f);
    camera.set_zoom(90.0f);
    camera.set_transform(CAMERA_POSITION, CAMERA_TARGET);

    blin_uniform_t blin_uniforms;
    
    blin_uniforms.diffuse_texture = &t_diffuse;
    blin_uniforms.normal_texture = &t_normal;
    
    blin_shader_t blin_shader;

    blin_shader.set_uniform(&blin_uniforms);
    
    while(!window_should_close(window)) {
        
        framebuffer.clear_color(background);
        framebuffer.clear_depth(1.0f);
        
        update_camera(window, &camera, &record);
        wall.set_rotation(wall_rotation);

        blin_uniforms.model_matrix = wall.get_model_matrix();
        blin_uniforms.model_matrix = wall.get_model_matrix();
        blin_uniforms.camera_pos = camera.get_position();
        blin_uniforms.proj_matrix = camera.get_projection_matrix();
        blin_uniforms.view_matrix = camera.get_view_matrix();

        blin_shader.set_uniform(&blin_uniforms);
        
        draw_triangle(&framebuffer, wall.get_vbo(), &blin_shader);
        draw_gui(window, &demo_gui);
        window_draw_buffer(window, &framebuffer);


        record.orbit_delta = vec2(0, 0);
        record.pan_delta = vec2(0, 0);
        record.dolly_delta = 0;
        record.single_click = 0;
        record.double_click = 0;
        input_poll_events();
    }       

    platform_terminate();

}
