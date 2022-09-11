#include <iostream>
#include <string>
#include "api.h"

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

    while(!window_should_close(window)) {
        input_poll_events();
        framebuffer.clear_color(background);
        
        draw_gui(window, &demo_gui);
        window_draw_buffer(window, &framebuffer);
    }

    platform_terminate();

}
