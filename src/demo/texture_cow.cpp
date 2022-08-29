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
    vec4 vertex_shader(const float* const vert, const std::vector<int>& offset, floatstream & varying) const {
        vec3 pos, norm;
        vec2 uv;
        getattr(vert, offset[0], pos);
        getattr(vert, offset[1], norm);
        getattr(vert, offset[2], uv);
        
        mat4 m, vp;
        getunif(0, m);
        getunif(1, vp);
        mat4 mvp = vp * m;
        mat3 mit = clip_translate(m).inv().T();

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

        vector<vec3> light_pos = {vec3(8.0f, 10.0f, -6.0f), vec3(-8.0f, 10.0f, -6.0f)};
        vector<vec3> light_intensity = {vec3(80.0f, 80.0f, 80.0f), vec3(80.0f, 80.0f, 80.0f)};

        vec3 ka = vec3(0.005, 0.005, 0.005);
        vec3 kd = vec3(t_color.x(), t_color.y(), t_color.z());
        vec3 ks = vec3(0.7937, 0.7937, 0.7937);

        vec3 amb_light_intensity(10.0f, 10.0f, 10.0f);
        float p = 200.0f;

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

            result_color = result_color + La + Ld + Ls;
        }

        return result_color;
    }
};


int main(int argc, char* argv[]) {
    vec3 clear_color = vec3(0.0f, 0.0f, 0.0f);
    mgl_init("hello rasterizer", 800, 600);
    mgl_clear_depth(1.0f);

    MyShader mshader;
    
    // load texture
    Model cow("asset/model/cow/cow.obj");
    cow.set_size(vec3(5.0f));
    
    // camera init
    PerspectiveCamera camera;
    vec3 translation(-6.0f, 1.0f, -8.0f);
    vec3 rotation(-141.0f, -3.0f, 0.0f);
    vec3 cow_rotation(0.0f);
    
    float near_plane = 1.0f, far_plane = 50.0f, fov = 70.0f, ratio = 800.0f / 600.0f;
    camera.set(&near_plane, &far_plane, &fov, &ratio);
    camera.set_rotation(rotation);

    do {
        mgl_clear_color(clear_color);
        mgl_clear(MGL_COLOR | MGL_DEPTH);
        
        static bool show_demo_window = true;
        ImGui::Begin("Control Pannel");
        ImGui::SliderFloat3("rotation", rotation.e, -180.0f, 180.0f);
        ImGui::SliderFloat3("translation", translation.e, -20.0f, 20.0f);
        ImGui::SliderFloat3("cow rotation", cow_rotation.e, -180.0f, 180.0f);
        ImGui::ColorEdit3("clear color", (float*)&clear_color);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        camera.set_translation(translation);
        camera.set_rotation(rotation);
        camera.transfer(&mshader);
        cow.set_rotation(cow_rotation);
        cow.draw(&mshader);

    } while(!mgl_update());
    mgl_quit();
    return 0;
}