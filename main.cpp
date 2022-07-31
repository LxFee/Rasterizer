#include <iostream>
#include <vector>
#include "Shader.h"
#include "mgl.h"
#include "model.h"
#include "Texture.h"
using namespace std;

int alocation_p;
int alocation_uv;
int ulocation_mvp = -1;
int tlocation_tex = -1;

vector<vec3> points = {
    vec3(-5.0f, 5.0f, 0.0f), 
    vec3(-5.0f, -5.0f, 0.0f),
    vec3(5.0f, -5.0f, 0.0f), 
    vec3(5.0f, 5.0f, 0.0f)};

vector<vec2> uv {
    vec2(0.0f, 1.0f), 
    vec2(0.0f, 0.0f), 
    vec2(1.0f, 0.0f),
    vec2(1.0f, 1.0f)};

vector<int> ind {
    0, 1, 2, 
    2, 3, 0};


class MyShader : public Shader {
    vec4 vertex_shader(int vbo, int index,floatstream & varying) const {
        vec3 pos = getattr(vbo, index, alocation_p);
        vec2 uv = getattr(vbo, index, alocation_uv);

        putvarying(varying, uv.e, 2);

        mat4 mvp = getunif(ulocation_mvp);
        vec4 fpos = mvp * vec4(pos, 1.0f);
        return fpos;
    }

    vec4 fragment_shader(floatstream& varying) const {
        int offset = 0;
        vec2 uv = getvaring(varying, 2, offset);
        vec4 fcolor = sample(tlocation_tex, uv.u(), uv.v());
        return fcolor;
    }
};


int main(int argc, char* argv[]) {
    mgl_init("hello rasterizer", 800, 600);
    Texture* t = Texture::readfromfile("assert/test.png");
    
    mgl_set_init_color(vec4(0.0f, 0.0f, 0.0f));
    mgl_set_init_zbuffer(1.0f);

    int vbo = mgl_create_vbo();
    int ebo = mgl_create_ebo();

    alocation_p = mgl_vertex_attrib_pointer(vbo, 3, (float*)points.data());
    alocation_uv = mgl_vertex_attrib_pointer(vbo, 2, (float*)uv.data());
    mgl_vertex_index_pointer(ebo, ind.size(), ind.data());

    MyShader mshader;
    
    mat4 P = perspective(1.0f, 50.0f, 90, 800.0f / 600.0f);
    mat4 M = translate(vec3(0.0f, 0.0f, -10.0f));
    ulocation_mvp = mshader.uniform((P * M).e, 16, ulocation_mvp);
    tlocation_tex = mshader.bindtexture(t, tlocation_tex);
    
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