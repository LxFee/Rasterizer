#pragma once
#include <vector>
#include "mathbase.h"
#include <cassert>

class Shader {
public:
    virtual vec4 vertex_shader(float *attrbegin, float *attrend, std::vector<float>& varying) const = 0;
    virtual vec4 fragment_shader(std::vector<float>& varying) const = 0;
    int uniform_vec2(vec2 v, int location = -1);
    int uniform_vec3(vec3 v, int location = -1);
    int uniform_vec4(vec4 v, int location = -1);
    int uniform_1f(float v, int location = -1);
    int uniform_mat4(mat4 m, int location = -1);

private:
    float getu1f(int location);
    vec4 getuvec4(int location);
    vec3 getuvec3(int location);
    vec2 getuvec2(int location);
    mat4 getumat4(int location);

    std::vector<float> uniforms;
    std::vector<int> uniforms_offset;
};

vec4 getavec4(float *attrbegin, int& offset);

vec3 getavec3(float *attrbegin, int& offset);

vec2 getavec2(float *attrbegin, int& offset);
