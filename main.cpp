#include <iostream>
#include <vector>
#include "Shader.h"
#include "mgl.h"
#include "model.h"
using namespace std;

int alocation_p;
int alocation_c;

int ulocation_mvp = -1;

vector<vec3> points {vec3(0.0f, 5.0f, 0.0f), vec3(5.0f, -5.0f, 0.0f), vec3(-5.0f, -5.0f, 0.0f)};

vector<int> ind {0, 2, 1};

vector<vec3> color {vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)};

class MyShader : public Shader {
    vec4 vertex_shader(int vbo, int index,floatstream & varying) const {
        vec3 pos = getattr(vbo, index, alocation_p);
        vec3 color = getattr(vbo, index, alocation_c);
        mat4 mvp = getunif(ulocation_mvp);

        putvarying(varying, color.e, 3);
        vec4 fpos = mvp * vec4(pos, 1.0f);
        return fpos;
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

    alocation_p = mgl_vertex_attrib_pointer(vbo, 3, (float*)points.data());
    alocation_c = mgl_vertex_attrib_pointer(vbo, 3, (float*)color.data());
    mgl_vertex_index_pointer(ebo, ind.size(), ind.data());

    MyShader mshader;
    
    float angle = 0;
    mat4 P = perspective(0.5f, 100.0f, 90.0f, 800.0f / 600.0f);
    mat4 M(1.0f);
    


    int uptime = 0;
    while(1) {
        mgl_clear(MGL_COLOR | MGL_DEPTH);
        angle += 1;
        M = translate(vec3(0.0f, 0.0f, -3.0f)) * rotate(vec3(0.0f, 1.0f, 0.0f), angle);
        ulocation_mvp = mshader.uniform((P * M).e, 16, ulocation_mvp);
        
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