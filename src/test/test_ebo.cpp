/**
 * @file test_ebo.cpp
 * @author limil (limil.top@qq.com)
 * @brief 测试ebo是否工作正常。正常应当显示一个正方形
 * @date 2022-09-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <iostream>
#include <vector>
#include "shader.h"
#include "mgl.h"
#include <cmath>

using namespace std;

class MyShader : public Shader {
    void vertex_shader(const float* const vert, const std::vector<int>& offset, V2FO & v2f) const {
        vec3 pos;
        vec2 uv;
        getattr(vert, offset[0], pos);
        getattr(vert, offset[1], uv);

        mat4 vp;
        getunif(0, vp);

        v2f.texcoord = uv;
        v2f.position = vp * vec4(pos, 1.0f);
    }

    void fragment_shader(const V2FI & v2f, F2B& f2b) const {
        vec2 uv;
        uv = v2f.texcoord;
        
        f2b.color0 = vec4(uv.u(), uv.v(), 0.5f, 1.0f);
    }
};

struct Vertex {
    vec3 postion;
    vec2 texcoord;
};

vector<Vertex> vertex {
    {vec3(-1, -1, -5), vec2(0, 0)},
    {vec3(1, -1, -5), vec2(1, 0)},
    {vec3(1, 1, -5), vec2(1, 1)},
    {vec3(-1, 1, -5), vec2(0, 1)}
};

vector<int> indexes{0, 1, 2, 0, 2, 3};

int main(int argc, char* argv[]) {
    
    mgl_init("hello rasterizer", 800, 600);
    mgl_clear_color(vec4(0.0f, 0.0f, 0.0f, 1.0f));
    mgl_clear_depth(1.0f);

    // shader init
    MyShader mshader;
    mshader.uniform(perspective(1.0f, 50.0f, 45.0f, 800.0f / 600.0f) * lookat(vec3(0.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f)), 0);
    
    // ebo
    int ebo = mgl_create_ebo(indexes.data(), indexes.size());
    int vbo = mgl_create_vbo(sizeof(Vertex), vertex.data(), vertex.size());
    mgl_vertex_attrib_pointer(vbo, 0, 3, 0);
    mgl_vertex_attrib_pointer(vbo, 1, 2, 3);

    do {
        mgl_clear(MGL_COLOR | MGL_DEPTH);

        mgl_draw(vbo, ebo, &mshader);
    } while(!mgl_update());
    mgl_quit();
    return 0;
}