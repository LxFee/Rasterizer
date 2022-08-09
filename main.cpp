#include <iostream>
#include <vector>
#include "Shader.h"
#include "mgl.h"
#include "model.h"
#include "Texture.h"
#include "camera.h"
#include <cmath>

using namespace std;

class MyShader : public Shader {
    vec4 vertex_shader(int vbo, int index,floatstream & varying) const {
        vec3 pos, norm;
        vec2 uv;
        getattr(vbo, index, 0, pos);
        getattr(vbo, index, 1, norm);
        getattr(vbo, index, 2, uv);
        
        mat4 m, vp;
        getunif(0, m);
        getunif(1, vp);
        mat4 mvp = vp * m;
        mat3 mit = clip_translate(m).inv().T();;

        vec4 point = m * vec4(pos);
        putvarying(varying, (mit * norm).normalized());
        putvarying(varying, uv);
        putvarying(varying, vec3(point.x(), point.y(), point.z()));

        vec4 fpos = mvp * vec4(pos, 1.0f);
        return fpos;
    }

    vec4 fragment_shader(floatstream& varying) const {
        int offset = 0;
        vec3 n, pos;
        vec2 uv;
        getvaring(varying, n, offset);
        getvaring(varying, uv, offset);
        getvaring(varying, pos, offset);
        n = n.normalized();

        vec3 camera_pos;
        getunif(2, camera_pos);
        
        vec4 t_color = sample(0, uv.u(), uv.v());
        vec4 nt_normal = sample(1, uv.u(), uv.v());
        n = vec3(nt_normal.x(), nt_normal.y(), nt_normal.z());
        n = (n * 2.0f - vec3(1.0f)).normalized();

        vector<vec3> light_pos = {vec3(3.0f, 2.0f, 4.0f)};
        vector<vec3> light_intensity = {vec3(40.0f, 40.0f, 40.0f)};

        vec3 ka = vec3(0.005, 0.005, 0.005);
        vec3 kd = vec3(t_color.x(), t_color.y(), t_color.z());
        vec3 ks = vec3(0.30, 0.30, 0.30);

        vec3 amb_light_intensity(10.0f, 10.0f, 10.0f);
        float p = 150.0f;

        vec3 view_dir = (camera_pos - pos).normalized();

        vec3 result_color(0.0f);
        for(int i = 0; i < light_pos.size(); i++) {
            vec3 light_dir = light_pos[i] - pos;
            float r2 = dot(light_dir, light_dir);
            light_dir = light_dir.normalized();
            vec3 h = (light_dir + view_dir).normalized();
            vec3 I = light_intensity[i] / r2;

            vec3 Ld = kd * I * max(0.0f, dot(n, light_dir));
            vec3 Ls = ks * I * pow(max(0.0f, dot(n, h)), p);
            vec3 La = ka * amb_light_intensity;

            result_color = result_color + Ld + Ls;
        }

        return result_color;
    }
};
vector<vec3> verts = {
    vec3(-1.0f, 1.0f, 0.0f),
    vec3(-1.0f, -1.0f, 0.0f),
    vec3(1.0f, -1.0f, 0.0f),
    vec3(1.0f, 1.0f, 0.0f)
};

vector<vec2> uvs = {
    vec2(0.0f, 1.0f),
    vec2(0.0f, 0.0f),
    vec2(1.0f, 0.0f),
    vec2(1.0f, 1.0f)
};

vector<vec3> normals = {
    vec3(0.0f, 0.0f, 1.0f),
    vec3(0.0f, 0.0f, 1.0f),
    vec3(0.0f, 0.0f, 1.0f),
    vec3(0.0f, 0.0f, 1.0f)
};

vector<int> inds = {0, 1, 2, 0, 2, 3};


int main(int argc, char* argv[]) {
    
    mgl_init("hello rasterizer", 800, 600);
    mgl_clear_color(vec4(0.0f, 0.0f, 0.0f));
    mgl_clear_depth(1.0f);

    // camera init
    PerspectiveCamera camera;
    camera.set_translation(vec3(-5.0f, 0.0f, 10.0f));
    float near_plane = 1.0f, far_plane = 50.0f, fov = 70.0f, ratio = 800.0f / 600.0f;
    camera.set(&near_plane, &far_plane, &fov, &ratio);

    // shader init
    MyShader mshader;

    // texture init
    Texture* t = Texture::readfromfile("asset/textures/brickwall.jpg");
    Texture* nt = Texture::readfromfile("asset/textures/brickwall_normal.jpg");
    mshader.bindtexture(t);
    mshader.bindtexture(nt);
    
    Model wall(verts, normals, uvs, inds);
    wall.set_size(vec3(5.0f));
    float angle = 0.0f;
    do {
        gui_newframe();

        angle += 1.0f;
        mgl_clear(MGL_COLOR | MGL_DEPTH);

        camera.transfer(&mshader);

        wall.draw(&mshader);

    } while(!mgl_update());
    mgl_quit();
    return 0;
}