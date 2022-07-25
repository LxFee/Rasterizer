#include "Shader.h"

int Shader::uniform_1f(float v, int location) {
    if(location >= uniforms_offset.size()) {
        return -1;
    }
    if(location < 0) {
        uniforms.emplace_back(v);
        int pre = 0;
        if(!uniforms_offset.empty()) pre = uniforms_offset.back();
        uniforms_offset.emplace_back(1 + pre);
        return uniforms_offset.size() - 1;
    }
    int pre = location > 0 ? uniforms_offset[location - 1] : 0;
    int cur = uniforms_offset[location];
    if(cur - pre != 1) return -1;
    uniforms[cur] = v;
    return location;
}

int Shader::uniform_vec2(vec2 v, int location) {
    if(location >= uniforms_offset.size()) {
        return -1;
    }
    if(location < 0) {
        uniforms.emplace_back(v.e[0]);
        uniforms.emplace_back(v.e[1]);
        int pre = 0;
        if(!uniforms_offset.empty()) pre = uniforms_offset.back();
        uniforms_offset.emplace_back(2 + pre);
        return uniforms_offset.size() - 1;
    }
    int pre = location > 0 ? uniforms_offset[location - 1] : 0;
    int cur = uniforms_offset[location];
    if(cur - pre != 2) return -1;
    uniforms[cur] = v.e[0];
    uniforms[cur + 1] = v.e[1];
    return location;
}

int Shader::uniform_vec3(vec3 v, int location) {
    if(location >= uniforms_offset.size()) {
        return -1;
    }
    if(location < 0) {
        uniforms.emplace_back(v.e[0]);
        uniforms.emplace_back(v.e[1]);
        uniforms.emplace_back(v.e[2]);
        int pre = 0;
        if(!uniforms_offset.empty()) pre = uniforms_offset.back();
        uniforms_offset.emplace_back(3 + pre);
        return uniforms_offset.size() - 1;
    }
    int pre = location > 0 ? uniforms_offset[location - 1] : 0;
    int cur = uniforms_offset[location];
    if(cur - pre != 3) return -1;
    uniforms[cur] = v.e[0];
    uniforms[cur + 1] = v.e[1];
    uniforms[cur + 2] = v.e[2];
    return location;
}

int Shader::uniform_vec4(vec4 v, int location) {
    if(location >= uniforms_offset.size()) {
        return -1;
    }
    if(location < 0) {
        uniforms.emplace_back(v.e[0]);
        uniforms.emplace_back(v.e[1]);
        uniforms.emplace_back(v.e[2]);
        uniforms.emplace_back(v.e[3]);
        int pre = 0;
        if(!uniforms_offset.empty()) pre = uniforms_offset.back();
        uniforms_offset.emplace_back(4 + pre);
        return uniforms_offset.size() - 1;
    }
    int pre = location > 0 ? uniforms_offset[location - 1] : 0;
    int cur = uniforms_offset[location];
    if(cur - pre != 4) return -1;
    uniforms[cur] = v.e[0];
    uniforms[cur + 1] = v.e[1];
    uniforms[cur + 2] = v.e[2];
    uniforms[cur + 3] = v.e[3];
    return location;
}

int Shader::uniform_mat4(mat4 m, int location) {
    if(location >= uniforms_offset.size()) {
        return -1;
    }
    if(location < 0) {
        for(int i = 0; i < 16; i++) uniforms.emplace_back(m.e[i]);
        int pre = 0;
        if(!uniforms_offset.empty()) pre = uniforms_offset.back();
        uniforms_offset.emplace_back(16 + pre);
        return uniforms_offset.size() - 1;
    }
    int pre = location > 0 ? uniforms_offset[location - 1] : 0;
    int cur = uniforms_offset[location];
    if(cur - pre != 16) return -1;
    for(int i = 0; i < 16; i++) uniforms[cur + i] = m.e[i];
    return location;
}

vec4 getavec4(float *attrbegin, int& offset) {
    offset += 4;
    return vec4(attrbegin[offset - 4], attrbegin[offset - 3], attrbegin[offset - 2], attrbegin[offset - 1]);
}

vec3 getavec3(float *attrbegin, int& offset) {
    offset += 3;
    return vec3(attrbegin[offset - 3], attrbegin[offset - 2], attrbegin[offset - 1]);
}

vec2 getavec2(float *attrbegin, int& offset) {
    offset += 2;
    return vec2(attrbegin[offset - 2], attrbegin[offset - 1]);
}


float Shader::getu1f(int location) {
    assert(location >= 0 && location < uniforms_offset.size());
    int pre = location > 0 ? uniforms_offset[location - 1] : 0;
    int cur = uniforms_offset[location];
    assert(cur - pre == 1);
    return uniforms[cur];
}

vec4 Shader::getuvec4(int location) {
    assert(location >= 0 && location < uniforms_offset.size());
    int pre = location > 0 ? uniforms_offset[location - 1] : 0;
    int cur = uniforms_offset[location];
    assert(cur - pre == 4);
    return vec4(uniforms[cur], uniforms[cur + 1], uniforms[cur + 2], uniforms[cur + 3]);
}

vec3 Shader::getuvec3(int location) {
    assert(location >= 0 && location < uniforms_offset.size());
    int pre = location > 0 ? uniforms_offset[location - 1] : 0;
    int cur = uniforms_offset[location];
    assert(cur - pre == 3);
    return vec3(uniforms[cur], uniforms[cur + 1], uniforms[cur + 2]);
}

vec2 Shader::getuvec2(int location) {
    assert(location >= 0 && location < uniforms_offset.size());
    int pre = location > 0 ? uniforms_offset[location - 1] : 0;
    int cur = uniforms_offset[location];
    assert(cur - pre == 2);
    return vec2(uniforms[cur], uniforms[cur + 1]);
}

mat4 Shader::getumat4(int location) {
    assert(location >= 0 && location < uniforms_offset.size());
    int pre = location > 0 ? uniforms_offset[location - 1] : 0;
    int cur = uniforms_offset[location];
    assert(cur - pre == 16);
    mat4 res;
    for(int i = 0; i < 16; i++) res.e[i] = uniforms[cur + i];
    return res;
}
