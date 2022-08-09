#pragma once
#include <vector>
#include "mathbase.h"
#include "Texture.h"
#include <cassert>

using floatstream = std::vector<float>;
extern float* mgl_query_vbo(int, int, int, int*);

struct uniform_element {
    uniform_element() : size(0) {}
    int size; // element size (num of float)
    floatstream value;
};

class Shader {
public:
    virtual vec4 vertex_shader(int vbo, int index,floatstream & varying) const = 0;
    virtual vec4 fragment_shader(floatstream& varying) const = 0;
    
    template<class T>
    void uniform(const T& unif, int location);

    void bindtexture(Texture *texture, int location);

protected:
    std::vector<uniform_element> uniforms;
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
void Shader::uniform(const T& unif, int location) {
    assert(sizeof(unif) % sizeof(float) == 0);
    int num = sizeof(unif) / sizeof(float);   
    assert(location >= 0);
    if(location >= uniforms.size()) uniforms.resize(location + 1);
    assert(uniforms[location].size == 0 || uniforms[location].size == num);
    uniforms[location].size = num;
    uniforms[location].value.resize(num);
    for(int i = 0; i < num; i++) {
        uniforms[location].value[i] = ((float*)&unif)[i];
    }
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
    assert(uniforms[location].size == num);
    unif = uniforms[location].value.data();
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

