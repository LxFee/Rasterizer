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
        vec3 bitangent, tangent;
        getattr(vert, offset[0], pos);
        getattr(vert, offset[1], norm);
        getattr(vert, offset[2], uv);
        getattr(vert, offset[3], tangent);


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
        // n = n.normalized();

        vec3 camera_pos;
        getunif(2, camera_pos);
        
        vec4 t_color = sample(0, uv.u(), uv.v());

        return t_color;
    }
};

int main(int argc, char* argv[]) {
    
    mgl_init("hello rasterizer", 800, 600);
    mgl_clear_color(vec4(0.0f, 0.0f, 0.0f));
    mgl_clear_depth(1.0f);

    // camera init
    PerspectiveCamera camera;
    float near_plane = 1.0f, far_plane = 50.0f, fov = 90.0f, ratio = 800.0f / 600.0f;
    camera.set(&near_plane, &far_plane, &fov, &ratio);

    // shader init
    MyShader mshader;
    
    Model wall("asset/model/brickwall/brickwall.obj");
    wall.set_size(vec3(5.0f));

    vec3 rotation(0.0f);
    vec3 wall_rotation(-22.0f, 75.0f, 15.0f);
    vec3 translation(0.0f, 0.0f, 4.0f);

    do {
        mgl_clear(MGL_COLOR | MGL_DEPTH);



        static bool show_demo_window = true;
        ImGui::Begin("Control Pannel");
        ImGui::SliderFloat3("rotation", rotation.e, -180.0f, 180.0f);
        ImGui::SliderFloat3("translation", translation.e, -20.0f, 20.0f);
        ImGui::SliderFloat3("wall rotation", wall_rotation.e, -180.0f, 180.0f);
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