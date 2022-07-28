#include <iostream>
#include <vector>
#include "Shader.h"
#include "mgl.h"
#include "model.h"
using namespace std;

int location_p;
int location_c;

vector<vec3> points {vec3(0.0f, 0.5f, 0.5f), vec3(-0.5f, -0.5f, 0.5f), vec3(0.5f, -0.5f, 0.5f)};

vector<int> ind {0, 1, 2};

vector<vec3> color {vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)};

class MyShader : public Shader {
    vec4 vertex_shader(int vbo, int index,floatstream & varying) const {
        vec3 pos = getattr(vbo, index, location_p);
        vec3 color = getattr(vbo, index, location_c);
        putvarying(varying, color.e, 3);
        return vec4(pos, 1.0f);
    }

    vec4 fragment_shader(floatstream& varying) const {
        int offset = 0;
        vec3 fcolor = getvaring(varying, 3, offset);
        return vec4(fcolor, 1.0f);
    }
};

int main(int argc, char* argv[]) {
    mgl_init("hello rasterizer", 800, 600);
    mgl_set_init_color(vec4(0.0f, 0.0f, 0.0f));
    mgl_set_init_zbuffer(1.0f);

    int vbo = mgl_create_vbo();
    int ebo = mgl_create_ebo();

    location_p = mgl_vertex_attrib_pointer(vbo, 3, (float*)points.data());
    location_c = mgl_vertex_attrib_pointer(vbo, 3, (float*)color.data());
    mgl_vertex_index_pointer(ebo, ind.size(), ind.data());

    MyShader mshader;

    int uptime = 0;
    while(1) {
        mgl_clear(MGL_COLOR | MGL_DEPTH);
        
        mgl_draw(vbo, ebo, &mshader);

        SDL_Event e;
        if (SDL_PollEvent(&e) & e.type == SDL_QUIT) {
            break;
        }
        int cur = SDL_GetTicks();
        cout << cur - uptime << endl;
        uptime = cur;

        mgl_update();
    }
    mgl_quit();
    return 0;
}