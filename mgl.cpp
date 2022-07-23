#include "mgl.h"

SDL_Window* window;
SDL_Renderer* renderer;
int width;
int height;
SDL_Texture* buffer;
static int* pixels;
static int pitch;

void mgl_init(const char *title, int w, int h) {
    SDL_Init(SDL_INIT_VIDEO);
    width = w;
    height = h;
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
    buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    SDL_LockTexture(buffer, NULL, (void**)&pixels, &pitch);
    if(title) SDL_SetWindowTitle(window, title);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_RENDERER_ACCELERATED);
    SDL_RenderClear(renderer);
}

void mgl_draw(Framebuffer* fb) {
    if(fb == NULL) {
        
    } else {

    }
}

void mgl_clear(vec4 col) {
    assert(width * 4 == pitch);
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            pixels[i * width + j] = packRGBA8888(col);
        }
    }
}

void mgl_swapbuffer() {
    SDL_UnlockTexture(buffer);
    SDL_RenderCopy(renderer, buffer, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_LockTexture(buffer, NULL, (void**)&pixels, &pitch);
}

void mgl_quit() {
    SDL_DestroyTexture(buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}