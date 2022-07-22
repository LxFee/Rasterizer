#include <iostream>
#include "SDL2/SDL.h"
using namespace std;

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer( 800, 600, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_RENDERER_ACCELERATED);
    SDL_RenderClear(renderer);
    
    SDL_Texture* buffer = SDL_CreateTexture(renderer,
                           SDL_PIXELFORMAT_BGRA8888,
                           SDL_TEXTUREACCESS_STREAMING, 
                           800,
                           600);
    char *pixels;
    int pitch;
    int k = 0;
    int uptime = 0;
    int d = 1;
    while(1) {
        k += d;
        if(k == 255) d = -1;
        if(k == 0) d = 1;
        SDL_LockTexture(buffer, NULL, (void **)&pixels, &pitch);
        for(int i = 0; i < 600; i++) {
            for(int j = 0; j < 800; j++) {
                pixels[4 * (i * 800 + j)] = k;
                pixels[4 * (i * 800 + j) + 1] = k;
                pixels[4 * (i * 800 + j) + 2] = k;
                pixels[4 * (i * 800 + j) + 3] = k;
            }
        }
        SDL_UnlockTexture(buffer);
        SDL_RenderCopy(renderer, buffer, NULL, NULL);
        SDL_RenderPresent(renderer);
        cout << SDL_GetTicks() - uptime << endl;
        uptime = SDL_GetTicks();
        SDL_Event e;
        if (SDL_PollEvent(&e) & e.type == SDL_QUIT) {
            break;
        }

    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}