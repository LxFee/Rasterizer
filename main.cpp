#include <iostream>
#include <vector>
#include "Shader.h"
#include "mgl.h"
#include "model.h"
#include "Texture.h"
using namespace std;

int alocation_p;
int alocation_n;
int alocation_uv;

int ulocation_mvp = -1;
int tlocation_tex = -1;

class MyShader : public Shader {
    vec4 vertex_shader(int vbo, int index,floatstream & varying) const {
        vec3 pos, norm;
        vec2 uv;
        getattr(vbo, index, alocation_p, pos);
        getattr(vbo, index, alocation_n, norm);
        getattr(vbo, index, alocation_uv, uv);
        
        putvarying(varying, norm);
        putvarying(varying, uv);
        putvarying(varying, pos);

        mat4 mvp;
        getunif(ulocation_mvp, mvp);
        vec4 fpos = mvp * vec4(pos, 1.0f);
        return fpos;
    }

    vec4 fragment_shader(floatstream& varying) const {
        int offset = 0;
        vec3 n, uv, pos;
        getvaring(varying, n, offset);
        getvaring(varying, uv, offset);
        getvaring(varying, pos, offset);
        
        return vec4((n.normalized() + vec3(1.0f)) / 2.0f, 1.0f);
        // return sample(tlocation_tex, uv.u(), uv.v());
    }
};


int main(int argc, char* argv[]) {
    mgl_init("hello rasterizer", 800, 600);
    mgl_set_init_color(vec4(0.0f, 0.0f, 0.0f));
    mgl_set_init_zbuffer(1.0f);

    MyShader mshader;

    Model cow("assert/cow/cow.obj");
    Texture* t = Texture::readfromfile("assert/cow/cow.png");
    tlocation_tex = mshader.bindtexture(t, tlocation_tex);
    
    int vbo = mgl_create_vbo();
    int ebo = mgl_create_ebo();

    alocation_p = mgl_vertex_attrib_pointer(vbo, 3, (float*)cow.verts.data());
    alocation_n = mgl_vertex_attrib_pointer(vbo, 3, (float*)cow.norms.data());
    alocation_uv = mgl_vertex_attrib_pointer(vbo, 2, (float*)cow.tex_coord.data());

    mgl_vertex_index_pointer(ebo, cow.nverts(), NULL);
    mat4 P = perspective(1.0f, 50.0f, 90, 800.0f / 600.0f);
    
    float angle = -20.0f;
    int uptime = 0;
    while(1) {
        angle += 0.5f;

        mgl_clear(MGL_COLOR | MGL_DEPTH);
        mat4 M = translate(vec3(0.0f, 0.0f, angle)) * scale(vec3(5.0f, 5.0f, 5.0f));// * rotate(vec3(0.0f, 1.0f, 0.0f), angle);
        ulocation_mvp = mshader.uniform(P * M, ulocation_mvp);

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