#include <iostream>
#include <vector>
#include <cmath>
#include "api.h"

using namespace std;

class MyShader : public Shader {
    void vertex_shader(const float* const vert, const std::vector<int>& offset, V2FO & v2f) const {
        vec3 pos, norm;
        vec2 uv;
        vec3 bitangent, tangent;
        getattr(vert, offset[0], pos);
        getattr(vert, offset[1], norm);
        getattr(vert, offset[2], uv);
        getattr(vert, offset[3], tangent);

        mat4 m, vp;
        getunif(0, m);
        getunif(1, vp);
        mat4 mvp = vp * m;
        mat3 mit = clip_translate(m).T().inv();


        vec3 T = (mit * tangent).normalized();
        vec3 N = (mit * norm).normalized();
        vec3 B = cross(N, T).normalized();
        mat3 TBN(T, B, N);

        vec4 point = m * vec4(pos, 1.0f);
        putvarying(v2f.varying, N);
        putvarying(v2f.varying, vec3(point.x(), point.y(), point.z()));
        putvarying(v2f.varying, TBN);

        v2f.texcoord = uv;
        v2f.position = mvp * vec4(pos, 1.0f);

    }

    void fragment_shader(const V2FI & v2f, F2B& f2b) const {
        int offset = 0;
        vec3 n, pos;
        vec2 uv;
        mat3 TBN;
        getvaring(v2f.varying, n, offset);
        getvaring(v2f.varying, pos, offset);
        getvaring(v2f.varying, TBN, offset);
        uv = v2f.texcoord;

        vec3 camera_pos;
        getunif(2, camera_pos);
        
        vec4 t_color = mgl_texture_sample2d(0, uv.u(), uv.v());
        vec4 nt_normal = mgl_texture_sample2d(1, uv.u(), uv.v());
        n = vec3(nt_normal.x(), nt_normal.y(), nt_normal.z());
        n = (n * 2.0f - vec3(1.0f)).normalized();
        n = (TBN * n).normalized();
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

        f2b.color0 = vec4(result_color, 1.0);
    }
};

int main(int argc, char* argv[]) {
    
    mgl_init("hello rasterizer", 800, 600);
    mgl_clear_color(vec4(0.0f, 0.0f, 0.0f, 1.0f));
    mgl_clear_depth(1.0f);

    // camera init
    PerspectiveCamera camera;
    float near_plane = 1.0f, far_plane = 50.0f, fov = 70.0f, ratio = 800.0f / 600.0f;
    camera.set(&near_plane, &far_plane, &fov, &ratio);

    // shader init
    MyShader mshader;
    Model wall("asset/model/brickwall/brickwall.obj");
    wall.set_size(vec3(5.0f));

    vec3 rotation(0.0f);
    vec3 wall_rotation(0.0f);
    vec3 translation(0.0f, 0.0f, 10.0f);

    do {
        mgl_clear(MGL_COLOR | MGL_DEPTH);
        static bool show_demo_window = true;
        ImGui::Begin("Control Pannel");
        ImGui::SliderFloat3("rotation", rotation.data(), -180.0f, 180.0f);
        ImGui::SliderFloat3("translation", translation.data(), -20.0f, 20.0f);
        ImGui::SliderFloat3("wall rotation", wall_rotation.data(), -180.0f, 180.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        camera.set_rotation(rotation);
        camera.set_translation(translation);
        camera.transfer(&mshader);
        wall.set_rotation(wall_rotation);
        wall.draw(&mshader);
    } while(!mgl_update());
    mgl_quit();
    return 0;
}