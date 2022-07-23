#pragma once
#include <vector>
#include "mathbase.h"
#include <cassert>

class Shader {
public:
    virtual vec4 vertex_shader(float *attrbegin, float *attrend, std::vector<float>& varying) const = 0;
    virtual vec4 fragment_shader(std::vector<float>& varying) const = 0;
    int uniform_vec2(int location, vec2 v);
    int uniform_vec3(int location, vec3 v);
    int uniform_vec4(int location, vec4 v);
    int uniform_1f(int location, float v);
    int uniform_mat4(int location, mat4 m);

private:
    float getu1f(int location);
    vec4 getuvec4(int location);
    vec3 getuvec3(int location);
    vec2 getuvec2(int location);
    mat4 getumat4(int location);

    std::vector<float> uniforms;
    std::vector<int> uniforms_check;
};

vec4 getavec4(float *attrbegin, int& offset);

vec3 getavec3(float *attrbegin, int& offset);

vec2 getavec2(float *attrbegin, int& offset);
