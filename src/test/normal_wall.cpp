#include <iostream>
#include <string>
#include "api.h"

using namespace std;

int main(int argc, char* argv[]) {
    platform_initialize();
    window_t *window = window_create("main window!", 800, 800);
    framebuffer_t framebuffer(800, 800);
    vec4 background;
    widget_t demo_gui {
        "normal wall", 
        {
            {
                "background", 
                {
                    {"color", ITEM_TYPE_COLOR, background.data()}
                }
            }
        }
    };

    while(!window_should_close(window)) {
        input_poll_events();
        framebuffer.clear_color(background);
        
        draw_gui(window, &demo_gui);
        window_draw_buffer(window, &framebuffer);
    }

    platform_terminate();

}
