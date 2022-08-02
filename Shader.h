#pragma once
#include <vector>
#include "mathbase.h"
#include "Texture.h"
#include <cassert>

using floatstream = std::vector<float>;
extern float* mgl_query_vbo(int, int, int, int*);

class Shader {
public:
    virtual vec4 vertex_shader(int vbo, int index,floatstream & varying) const = 0;
    virtual vec4 fragment_shader(floatstream& varying) const = 0;
    
    template<class T>
    int uniform(const T& unif, int location = -1);

    int bindtexture(Texture *texture, int location = -1);

protected:
    std::vector<float> uniforms;
    std::vector<int> uniforms_offset;
    std::vector<Texture*> textures;

    vec4 sample(int texture_location, float u, float v) const ;

    template<class T>
    void getunif(int location, T& unif) const ;

    template<class T>
    void getattr(int vbo, int index, int location, T& attr) const ;

    template<class T>
    static void putvarying(floatstream& varying, const T& var);
    
    template<class T>
    static void getvaring(floatstream& varying, T& var, int& offset); 
};

template<class T>
int Shader::uniform(const T& unif, int location) {
    assert(sizeof(unif) % sizeof(float) == 0);
    int num = sizeof(unif) / sizeof(float);   
    assert(location < (int)uniforms_offset.size());
    if(location < 0) {
        for(int i = 0; i < num; i++) uniforms.push_back(((float*)&unif)[i]);
        int pre = uniforms_offset.empty() ? 0 : uniforms_offset.back();
        uniforms_offset.emplace_back(num + pre);
        return (int)uniforms_offset.size() - 1;
    }
    int pre = location > 0 ? uniforms_offset[location - 1] : 0;
    int cur = uniforms_offset[location];
    assert(cur - pre == num);
    for(int i = 0; i < num; i++) uniforms[pre + i] = ((float*)&unif)[i];
    return location;
}

template<class T>
void Shader::putvarying(floatstream& varying, const T& var) {
    assert(sizeof(var) % sizeof(float) == 0);
    int num = sizeof(var) / sizeof(float);
    for(int i = 0; i < num; i++) {
        varying.push_back(((float*)&var)[i]);
    }
}

template<class T>
void Shader::getvaring(floatstream& varying, T& var, int& offset) {
    assert(sizeof(var) % sizeof(float) == 0);
    int num = sizeof(var) / sizeof(float);
    assert(offset + num <= varying.size());
    var = varying.data() + offset;
    offset += num;
}

template<class T>
void Shader::getunif(int location, T& unif) const {
    assert(sizeof(unif) % sizeof(float) == 0);
    int num = sizeof(unif) / sizeof(float);
    assert(location >= 0 && location < (int)uniforms.size());
    int offset = 0;
    if(location) offset = uniforms_offset[location - 1];
    int len = uniforms_offset[location] - offset;
    assert(len == num);
    unif = uniforms.data() + offset;
}

template<class T>
void Shader::getattr(int vbo, int index, int location, T& attr) const {
    assert(sizeof(attr) % sizeof(float) == 0);
    int num = sizeof(attr) / sizeof(float);
    int len;
    float *vbop;
    assert((vbop = mgl_query_vbo(vbo, index, location, &len)) != NULL);
    assert(len == num);
    attr = vbop;
}

