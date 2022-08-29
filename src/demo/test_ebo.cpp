#include <iostream>
#include <vector>
#include "shader.h"
#include "mgl.h"
#include "model.h"
#include "texture.h"
#include "camera.h"
#include <cmath>

using namespace std;

class MyShader : public Shader {
    vec4 vertex_shader(const float* const vert, const std::vector<int>& offset, floatstream & varying) const {
        vec3 pos;
        getattr(vert, offset[0], pos);

        mat4 m, vp;
        getunif(1, vp);
        mat4 mvp = vp;

        vec4 fpos = mvp * vec4(pos, 1.0f);
        return fpos;
    }

    vec4 fragment_shader(const floatstream& varying) const {
        return vec4(1.0f);
    }
};

vector<vec3> vertex {
    vec3(-1, -1, -5),
    vec3(1, -1, -5),
    vec3(1, 1, -5),
    vec3(-1, 1, -5)
};

vector<int> indexes{0, 1, 2, 0, 2, 3};

int main(int argc, char* argv[]) {
    
    mgl_init("hello rasterizer", 800, 600);
    mgl_clear_color(vec4(0.0f, 0.0f, 0.0f, 1.0f));
    mgl_clear_depth(1.0f);

    // camera init
    PerspectiveCamera camera;
    float near_plane = 1.0f, far_plane = 50.0f, fov = 90.0f, ratio = 800.0f / 600.0f;
    camera.set(&near_plane, &far_plane, &fov, &ratio);

    // shader init
    MyShader mshader;

    int ebo = mgl_create_ebo(indexes.data(), indexes.size());
    int vbo = mgl_create_vbo(sizeof(vec3), vertex.data(), vertex.size());
    mgl_vertex_attrib_pointer(vbo, 0, 3, 0);

    do {
        mgl_clear(MGL_COLOR | MGL_DEPTH);

        camera.transfer(&mshader);

        mgl_draw(vbo, ebo, &mshader);
    } while(!mgl_update());
    mgl_quit();
    return 0;
}