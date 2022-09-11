#include "platform.h"
#include <iostream>
#include <string>

using namespace std;

void button_callback(window_t *window, button_t button, int pressed) {
    if(pressed) cout << "Pressed ";
    else cout << "Released ";
    if(button == BUTTON_L) {
        cout << "L: ";
    } else {
        cout << "R: ";
    }
    cout << (const char*)window_get_userdata(window) << endl;
}

void key_callback(window_t *window, keycode_t key, int pressed) {
    if(pressed) cout << "Pressed ";
    else cout << "Released ";
    switch(key) {
        case KEY_A:
            cout << "A: "; break;
        case KEY_S:
            cout << "S: "; break;
        case KEY_W:
            cout << "W: "; break;
        case KEY_D:
            cout << "D: "; break;
        case KEY_SPACE:
            cout << "SPACE: "; break;
    }
    cout << (const char*)window_get_userdata(window) << endl;
}

void scroll_callback(window_t *window, float offset) {
    cout << "offset: " << offset << " ";
    cout << (const char*)window_get_userdata(window) << endl;
}

int main(int argc, char* argv[]) {
    platform_initialize();

    window_t *window1 = window_create("main window!", 800, 800);
    window_t *window2 = window_create("hello window!", 800, 800);
    string msg1 = "this is window1";
    string msg2 = "this is window2";
    window_set_userdata(window1, msg1.data());
    window_set_userdata(window2, msg2.data());

    callbacks_t callbacks;
    callbacks.button_callback = button_callback;
    callbacks.key_callback = key_callback;
    callbacks.scroll_callback = scroll_callback;

    input_set_callbacks(window1, callbacks);
    input_set_callbacks(window2, callbacks);

    float prev = platform_get_time();
    while(window1 || window2) {
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
        input_poll_events();
    }

    platform_terminate();

}
