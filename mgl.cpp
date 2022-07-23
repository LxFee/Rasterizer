#include "mgl.h"

SDL_Window* window;
SDL_Renderer* renderer;
int width;
int height;
SDL_Texture* buffer;
std::vector<float> zbuffer;
static int* pixels;
static int pitch;

struct Component {
    Component(int num, float * data) : num(num), data(data) {}
    int num; // num of float
    float *data;
};

struct VBO {
    VBO() : total_num(0) {}
    int total_num; // num of float
    std::vector<Component> data;
};


std::vector<VBO> vbos;

int mgl_create_vbo() {
    vbos.emplace_back();
    return (int)vbos.size() - 1;
}

bool mgl_vertex_attrib_pointer(int vbo_ind, int num, float* data) {
    if(vbo_ind < 0 || vbo_ind >= vbos.size()) return false;
    VBO &vbo = vbos[vbo_ind];
    vbo.data.emplace_back(num, data);
    vbo.total_num += num;
    return true;
}


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

void mgl_draw(int vbo, int count, Shader* shader, Framebuffer* fb) {
    
}

void mgl_clear(vec4 col, Framebuffer *fb) {
    if(fb == NULL) {
        assert(width * 4 == pitch);
        for(int i = 0; i < height; i++) {
            for(int j = 0; j < width; j++) {
                pixels[i * width + j] = packRGBA8888(col);
            }
        }
    } else {
        fb->clear(col);
    }
}

void mgl_update(Framebuffer* fb) {
    if(fb == NULL) {
        SDL_UnlockTexture(buffer);
        SDL_RenderCopy(renderer, buffer, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_LockTexture(buffer, NULL, (void**)&pixels, &pitch);
    } else {
        fb->update();
    }
}

void mgl_quit() {
    SDL_DestroyTexture(buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}