#include "platform.h"
#include <cassert>
#include <vector>
#include <iostream>
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_sdlrenderer.h"
#include "image.h"


struct window {
    /* sdl data */
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *surface;
    void* pixels;
    int width, height, pitch;
    /* imgui data */
    ImGuiContext *ctx;
    /* common data */
    int should_close;
    char keys[KEY_NUM];
    char buttons[BUTTON_NUM];
    callbacks_t callbacks;
    void *userdata;
};

namespace {
    std::vector<window_t*>& windows() {
        static std::vector<window_t*> window_entities;
        return window_entities;
    }

    void gui_new_frame() {
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    void gui_create_context(window_t* window) {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        window->ctx = ImGui::CreateContext();
        ImGui::SetCurrentContext(window->ctx);
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        // Setup Platform/Renderer backends
        ImGui_ImplSDL2_InitForSDLRenderer(window->window, window->renderer);
        ImGui_ImplSDLRenderer_Init(window->renderer);

        gui_new_frame();
    }

    void gui_render(window_t *window) {
        ImGui::SetCurrentContext(window->ctx);
        ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        gui_new_frame();
    }

    void gui_destroy(window_t *window) {
        ImGui::SetCurrentContext(window->ctx);
        ImGui_ImplSDLRenderer_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext(window->ctx);
    }

    window_t* query_window(int window_id) {
        for(auto& window : windows()) {
            if(!window) continue;
            if(SDL_GetWindowID(window->window) == window_id) {
                return window;
            }
        }   
        return NULL;
    }
    
    window_t *query_window_from_event(SDL_Event* event) {
        window_t *window = NULL;
        switch(event->type) {
            case SDL_WINDOWEVENT:
                window = query_window(event->window.windowID);
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                window = query_window(event->button.windowID);
                break;
            case SDL_MOUSEWHEEL:
                window = query_window(event->wheel.windowID);
                break;
            case SDL_MOUSEMOTION:
                window = query_window(event->motion.windowID);
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                window = query_window(event->key.windowID);
                break;
        }
        return window;
    }

    bool gui_process_event(SDL_Event* event) {
        window_t *window = query_window_from_event(event);
        
        bool in_gui = false;
        if(!window) {
            for(auto curr_window : windows()) {
                if(!curr_window) continue;
                ImGui::SetCurrentContext(curr_window->ctx); 
                ImGui_ImplSDL2_ProcessEvent(event);
            }
        } else {
            const ImGuiIO& io = ImGui::GetIO();
            /** 当指针在gui内，就不要将事件交给callback，防止操作冲突 **/
            /** https://www.cnblogs.com/cyds/p/16183345.html **/
            in_gui |= io.WantCaptureMouse;
            
            ImGui::SetCurrentContext(window->ctx); 
            ImGui_ImplSDL2_ProcessEvent(event);
        }
        return in_gui;
    }

    void handle_key_event(window_t *window, int virtual_key, char pressed) {
        callbacks_t callbacks = window->callbacks;
        keycode_t key;
        switch (virtual_key) {
            case SDLK_a:     key = KEY_A;     break;
            case SDLK_d:     key = KEY_D;     break;
            case SDLK_s:     key = KEY_S;     break;
            case SDLK_w:     key = KEY_W;     break;
            case SDLK_SPACE: key = KEY_SPACE; break;
            default:       key = KEY_NUM;
        }
        if(key < KEY_NUM) {
            window->keys[key] = pressed;
            if(!callbacks.key_callback) return ;
            callbacks.key_callback(window, key, pressed);
        }
    }  

    void handle_button_event(window_t *window, int xbutton, char pressed) {
        callbacks_t callbacks = window->callbacks;
        button_t button;
        switch (xbutton) {
            case SDL_BUTTON_LEFT:   button = BUTTON_L;     break;
            case SDL_BUTTON_RIGHT:  button = BUTTON_R;     break;
            default:                button = BUTTON_NUM;
        }
        if(button < BUTTON_NUM) {
            window->buttons[button] = pressed;
            if(!callbacks.button_callback) return ;
            callbacks.button_callback(window, button, pressed);
        }
    }

    void handle_wheel_event(window_t *window, float offset) {
        callbacks_t callbacks = window->callbacks;
        if(!callbacks.scroll_callback) return ;
        callbacks.scroll_callback(window, offset);
    }
}

/* platform initialization */
void platform_initialize(void) {
    assert(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) == 0);
}

void platform_terminate(void) {
    for(auto window : windows()) {
        if(!window) continue;
        window_destroy(window);
    }
    SDL_Quit();
}

window_t *window_create(const char *title, int width, int height) {
    window_t *window;
    
    assert(width > 0 && height > 0);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* sdl_window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, window_flags);
    SDL_Renderer* renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    SDL_Texture* surface = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    int *pixels, pitch;
    SDL_LockTexture(surface, NULL, (void**)&pixels, &pitch);

    assert(4 * width == pitch);
    if(title) SDL_SetWindowTitle(sdl_window, title);
    
    window = new window_t;
    memset(window, 0, sizeof(struct window));
    window->window = sdl_window;
    window->surface = surface;
    window->pixels = pixels;
    window->renderer = renderer;
    window->should_close = false;
    window->pitch = pitch;
    window->width = width;
    window->height = height;
    windows().push_back(window);
    
    gui_create_context(window);

    return window;
}

void window_destroy(window_t *window) {
    if(!window) return ;
    gui_destroy(window);
    SDL_DestroyTexture(window->surface);
    SDL_DestroyRenderer(window->renderer);
    SDL_DestroyWindow(window->window);
    for(auto& win : windows()) {
        if(win == window) {
            delete win;
            win = NULL;
        }
    }
}

bool window_should_close(window_t *window) {
    if(!window) return true;
    return window->should_close;
}

void window_set_userdata(window_t *window, void *userdata) {
    if(!window) return ;
    window->userdata = userdata;
}

void *window_get_userdata(window_t *window) {
    if(!window) return NULL;
    return window->userdata;
}

void *window_get_gui_context(window_t *window) {
    if(!window) return NULL;
    return window->ctx;
}

void window_draw_buffer(window_t *window, framebuffer_t* buffer) {
    if(!window) return ;
    int width = buffer->get_width(), height = buffer->get_height();
    assert(width == window->width && height == window->height);
    memcpy(window->pixels, buffer->get_color_data(), width * height * 4);
    SDL_UnlockTexture(window->surface);
    SDL_LockTexture(window->surface, NULL, (void**)&(window->pixels), &(window->pitch));
    SDL_RenderCopy(window->renderer, window->surface, NULL, NULL);
    gui_render(window);
    SDL_RenderPresent(window->renderer);
}

/* input related functions */
void input_poll_events(void) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            for(auto window : windows()) {
                if(!window) continue;
                window->should_close = true;
            }
            break;
        }
        
        if(gui_process_event(&event)) continue;

        window_t *window = query_window_from_event(&event);
        if(!window) continue;

        /* 窗口事件 */
        if(event.type == SDL_WINDOWEVENT) {
            switch(event.window.event) {
                case SDL_WINDOWEVENT_CLOSE:
                    window->should_close = true;
                    break;
            }
        }
        switch(event.type) {
            case SDL_MOUSEBUTTONDOWN:
                handle_button_event(window, event.button.button, 1);
                break;
            case SDL_MOUSEBUTTONUP:
                handle_button_event(window, event.button.button, 0);
                break;
            case SDL_MOUSEWHEEL:
                handle_wheel_event(window, event.wheel.preciseY);
                break;
            case SDL_KEYDOWN:
                handle_key_event(window, event.key.keysym.sym, 1);
                break;
            case SDL_KEYUP:
                handle_key_event(window, event.key.keysym.sym, 0);
                break;
        }
    }
}


int input_key_pressed(window_t *window, keycode_t key) {
    if(!window) return -1;
    assert(key >= 0 && key < KEY_NUM);
    return window->keys[key];
}

int input_button_pressed(window_t *window, button_t button) {
    if(!window) return -1;
    assert(button >= 0 && button < BUTTON_NUM);
    return window->buttons[button];
}

void input_query_cursor(window_t *window, float *xpos, float *ypos) {
    if(!window) return ;
    if(window->window == SDL_GetMouseFocus()) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        *xpos = (float)x;
        *ypos = (float)y;
    } else {
        *xpos = -1;
        *ypos = -1;
    }
}

void input_set_callbacks(window_t *window, callbacks_t callbacks) {
    if(!window) return ;
    window->callbacks = callbacks;
}


/* misc platform functions */
float platform_get_time(void) {
    return SDL_GetTicks() / 1000.0f;
}