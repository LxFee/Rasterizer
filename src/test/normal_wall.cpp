#include <iostream>
#include <string>
#include "api.h"
#include "blin_shader.h"

using namespace std;

int main(int argc, char* argv[]) {
    platform_initialize();
    int w = 800, h = 600;
    window_t *window = window_create("main window!", w, h);
    framebuffer_t framebuffer(w, h);
    vec4 background;
    widget_t demo_gui {
        "normal wall", 
        {
            {
                "background", 
                {
                    {"color", ITEM_TYPE_COLOR4, background.data()}
                }
            }
        }
    };

    mesh_t wall("assets/model/brickwall/brickwall.obj");
    wall.set_size(vec3(0.5f));
    texture_t t_diffuse("assets/model/brickwall/brickwall_diffuse.jpg", USAGE_LDR_COLOR);
    texture_t t_normal("assets/model/brickwall/brickwall_normal.jpg", USAGE_LDR_COLOR);

    pinned_camera_t camera(800.0f / 600.0f, PROJECTION_MODE_PERSPECTIVE);
    camera.set_near(0.1f);
    camera.set_far(1000.0f);
    camera.set_zoom(70.0f);
    camera.set_transform(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f));


    blin_uniform_t blin_uniforms;
    
    blin_uniforms.camera_pos = camera.get_position();
    blin_uniforms.diffuse_texture = &t_diffuse;
    blin_uniforms.normal_texture = &t_normal;
    blin_uniforms.model_matrix = wall.get_model_matrix();
    blin_uniforms.proj_matrix = camera.get_projection_matrix();
    blin_uniforms.view_matrix = camera.get_view_matrix();
    
    blin_shader_t blin_shader;

    blin_shader.set_uniform(&blin_uniforms);
    
    while(!window_should_close(window)) {
        input_poll_events();
        framebuffer.clear_color(background);
        framebuffer.clear_depth(1.0f);
        
        draw_triangle(&framebuffer, wall.get_vbo(), &blin_shader);

        draw_gui(window, &demo_gui);
        window_draw_buffer(window, &framebuffer);
    }

    platform_terminate();

}
