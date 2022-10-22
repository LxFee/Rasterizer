#include <iostream>
#include <string>

#include "core/api.h"
#include "utils/EventManager.h"
#include "utils/GStorage.h"

using namespace std;

void register_input(window_t *window);
void gui(window_t* window);

int main(int argc, char *argv[]) {
    platform_initialize();

    window_t *window1 = window_create("main window!", 800, 800);
    window_t *window2 = window_create("hello window!", 800, 800);
    string msg1 = "this is window1";
    string msg2 = "this is window2";
    
    GStorage<window_t*, string>::registerItem(window1, msg1);
    GStorage<window_t*, string>::registerItem(window2, msg2);

    register_input(window1);
    register_input(window2);
    EventManager::registerEvent(SDLK_ESCAPE | Events::KEYBOARD_PRESS, [](window_t* window){
        window_close(window);
    });

    float prev = platform_get_time();

    float num = 10.0f;

    framebuffer_t framebuffer(800, 800);
    framebuffer.clear_color(vec4(0.15f, 0.5f, 0.5f, 1.0f));
    vec4 col;
    col = framebuffer.get_color(0, 3);
    cout << col.r() << " " << col.g() << " " << col.b() << " " << col.a() << endl;
    while(window1 || window2) {
        input_poll_events();
        float curr = platform_get_time();
        if(curr - prev >= 1) {
            prev = curr;
            float x, y;
            if(window1) {
                cout << "window1 cursor: ";
                input_query_cursor(window1, &x, &y);
                cout << x << " " << y << endl;
            }
            if(window2) {
                cout << "window2 cursor: ";
                input_query_cursor(window2, &x, &y);
                cout << x << " " << y << endl;
            }
        }

        if(window1) {
            if(window_should_close(window1)) {
                window_destroy(window1);
                window1 = NULL;
            }
        }
        if(window2) {
            if(window_should_close(window2)) {
                window_destroy(window2);
                window2 = NULL;
            }
        }

        gui(window1);
        gui(window2);
        window_draw_buffer(window1, &framebuffer);
        window_draw_buffer(window2, &framebuffer);
    }

    platform_terminate();
    return 0;
}


void register_input(window_t *window) {
    EventManager::registerEvent(window_get_id(window) | Events::MOUSE_BUTTON_ANY | Events::MOUSE_BUTTON_TRIGGER, [](window_t* window, int button, int status){
        std::string& title = *GStorage<window_t*, std::string>::getPtr(window);
        cout << title << ": ";
        if(button == Events::MOUSE_BUTTON_LEFT) {
            cout << "L: ";
        } else {
            cout << "R: ";
        }
        if(status == Events::MOUSE_BUTTON_PRESS) {
            cout << "Pressed ";
        } else {
            cout << "Released ";
        }
        cout << endl;
    });

    EventManager::registerEvent(window_get_id(window) | Events::KEYBOARD_ANYKEY | Events::KEYBOARD_TRIGGER, [](window_t* window, int key, int status){
        std::string& title = *GStorage<window_t*, std::string>::getPtr(window);
        cout << title << ": ";

        switch(key) {
            case SDLK_a:
                cout << "A: ";
                break;
            case SDLK_s:
                cout << "S: ";
                break;
            case SDLK_w:
                cout << "W: ";
                break;
            case SDLK_d:
                cout << "D: ";
                break;
            case SDLK_SPACE:
                cout << "SPACE: ";
                break;
        }
        if(status == Events::KEYBOARD_PRESS) {
            cout << "Pressed ";
        } else {
            cout << "Released ";
        }
        cout << endl;
    });

    EventManager::registerEvent(window_get_id(window) | Events::MOUSE_SCROLL, [](window_t* window, float offset){
        std::string& title = *GStorage<window_t*, std::string>::getPtr(window);
        cout << title << ": ";
        cout << "offset: " << offset << " ";
        cout << endl;
    });
}

void gui(window_t* window) {
    if(!window) return;
    ImGuiContext* ctx = (ImGuiContext*)window_get_gui_context(window);
    if(!ctx) return;
    int id = 0;
    ImGui::SetCurrentContext(ctx);
    std::string& title = *GStorage<window_t*, std::string>::getPtr(window);
    ImGui::Begin(title.c_str());
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}