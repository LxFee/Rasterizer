#include "mgl.h"

SDL_Window* window;
SDL_Renderer* renderer;
int width;
int height;
static int* pixels;
static int pitch;

vec4 init_col;
SDL_Texture* buffer;
float init_z;
std::vector<float> zbuffer;

struct Component {
    Component(int size, float * data) : size(size), data(data) {}
    int size; // element size (num of float)
    float *data;
};

struct VBO {
    VBO() : total_size(0) {}
    int total_size; // num of float
    std::vector<Component> format;
};

struct EBO {
    EBO() : count(0), ind(NULL) {}
    int count;
    int* ind;
};

std::vector<VBO> vbos;
std::vector<EBO> ebos;

int mgl_create_vbo() {
    vbos.emplace_back();
    return (int)vbos.size() - 1;
}

int mgl_create_ebo() {
    ebos.emplace_back();
    return (int)ebos.size() - 1;
}

float* mgl_query_vbo(int vbo_ind, int index, int location, int* size) {
    if(vbo_ind < 0 || vbo_ind >= vbos.size()) return NULL;
    VBO& vbo = vbos[vbo_ind];
    if(location < 0 || location >= vbo.format.size()) return NULL;
    Component& comp = vbo.format[location];
    if(size) *size = comp.size;
    return comp.data + (*size) * index;
}

int mgl_vertex_attrib_pointer(int vbo_ind, int size, float* data) {
    if(vbo_ind < 0 || vbo_ind >= vbos.size()) return -1;
    VBO &vbo = vbos[vbo_ind];
    vbo.format.emplace_back(size, data);
    vbo.total_size += size;
    return (int)vbo.format.size() - 1;
}

int mgl_vertex_index_pointer(int ebo_ind, int count, int* ind) {
    if(ebo_ind < 0 || ebo_ind >= vbos.size()) return -1;
    EBO &ebo = ebos[ebo_ind];
    ebo.ind = ind;
    ebo.count = count;
    return count;
}


void mgl_init(const char *title, int w, int h) {
    SDL_Init(SDL_INIT_VIDEO);
    width = w;
    height = h;
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
    buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    zbuffer.resize(w * h);
    SDL_LockTexture(buffer, NULL, (void**)&pixels, &pitch);
    if(title) SDL_SetWindowTitle(window, title);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_RENDERER_ACCELERATED);
    SDL_RenderClear(renderer);
}

void mgl_draw(int vbo_ind, int ebo_ind, Shader* shader) {
    if(vbo_ind < 0 || vbo_ind >= vbos.size()) return ;
    if(ebo_ind < 0 || ebo_ind >= ebos.size()) return ;
    
    VBO &vbo = vbos[vbo_ind];
    // shader->vertex_shader()
}

void mgl_setinitz(float z) {
    init_z = z;
}

void mgl_setinitcol(vec4 col) {
    init_col = col;
}

void mgl_clear(int flag) {
    assert(width * 4 == pitch);
    if(flag & MGL_COLOR) {
        for(int i = 0; i < height; i++) {
            for(int j = 0; j < width; j++) {
                pixels[i * width + j] = packRGBA8888(init_col);
            }
        }
    }
    if(flag & MGL_DEPTH) {
        std::fill(zbuffer.begin(), zbuffer.end(), init_z);
    }
}

void mgl_update() {
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