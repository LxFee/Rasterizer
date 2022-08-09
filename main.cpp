#include <iostream>
#include <vector>
#include "Shader.h"
#include "mgl.h"
#include "model.h"
#include "Texture.h"
#include <cmath>

using namespace std;

int alocation_p;
int alocation_n;
int alocation_uv;

int ulocation_mvp = -1;
int ulocation_mit = -1;
int ulocation_m = -1;
int ulocation_mvp_emp = -1;

int tlocation_tex = -1;
int ntlocation_tex = -1;

vec3 camera_pos(-5.0f, 0.0f, 10.0f);

class MyShader : public Shader {
    vec4 vertex_shader(int vbo, int index,floatstream & varying) const {
        vec3 pos, norm;
        vec2 uv;
        getattr(vbo, index, alocation_p, pos);
        getattr(vbo, index, alocation_n, norm);
        getattr(vbo, index, alocation_uv, uv);
        
        mat4 mvp, m;
        mat3 mit;
        getunif(ulocation_mvp, mvp);
        getunif(ulocation_mit, mit);
        getunif(ulocation_m, m);

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
        
        vec4 t_color = sample(tlocation_tex, uv.u(), uv.v());
        vec4 nt_normal = sample(ntlocation_tex, uv.u(), uv.v());
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

    MyShader mshader;

    // texture
    Texture* t = Texture::readfromfile("asset/textures/brickwall.jpg");
    Texture* nt = Texture::readfromfile("asset/textures/brickwall_normal.jpg");
    tlocation_tex = mshader.bindtexture(t, tlocation_tex);
    ntlocation_tex = mshader.bindtexture(nt, ntlocation_tex);
    
    // attr
    int vbo = mgl_create_vbo();
    int ebo = mgl_create_ebo();
    alocation_p = mgl_vertex_attrib_pointer(vbo, 3, (float*)verts.data());
    alocation_n = mgl_vertex_attrib_pointer(vbo, 3, (float*)normals.data());
    alocation_uv = mgl_vertex_attrib_pointer(vbo, 2, (float*)uvs.data());
    mgl_vertex_index_pointer(ebo, inds.size(), inds.data());
    
    float angle = 0.0f;
    do {
        gui_newframe();

        angle += 1.0f;
        mgl_clear(MGL_COLOR | MGL_DEPTH);
        // MVP
        mat4 M = scale(vec3(5.0f, 5.0f, 5.0f));
        mat4 V = lookat(vec3(10.0f * sin(radian(angle)), 0.0f, 10.0f * cos(radian(angle))), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
        mat4 P = perspective(1.0f, 50.0f, 70, 800.0f / 600.0f);
        mat3 MIT = clip_translate(M).inv().T();
    
        // uniform
        ulocation_mvp = mshader.uniform(P * V * M, ulocation_mvp);
        ulocation_mit = mshader.uniform(MIT, ulocation_mit);
        ulocation_m = mshader.uniform(M, ulocation_m);

        mgl_draw(vbo, ebo, &mshader);

    } while(!mgl_update());
    mgl_quit();
    return 0;
}