#pragma once
#include <vector>
#include "mathbase.h"
#include "Texture.h"
#include <cassert>

using floatstream = std::vector<float>;
extern float* mgl_query_vbo(int, int, int, int*);

struct uniform_element {
    uniform_element() : size(0) {}
    int size; // 单位：float（4字节）
    floatstream value;
};

class Shader {
public:
    virtual vec4 vertex_shader(const float* const vert, const std::vector<int>& offset, floatstream & varying) const = 0;
    virtual vec4 fragment_shader(floatstream& varying) const = 0;
    
    template<class T>
    void uniform(const T& unif, int location);

    void bindtexture(Texture *texture, int location);

    virtual ~Shader();
protected:
    std::vector<uniform_element> uniforms;
    std::vector<Texture*> textures;

    vec4 sample(int texture_location, float u, float v) const ;

    template<class T>
    inline void getunif(int location, T& unif) const {
        assert( (sizeof(unif) % sizeof(float) == 0) && 
                (location >= 0 && location < (int)uniforms.size()) && 
                (uniforms[location].size == sizeof(unif) / sizeof(float)));
        unif = uniforms[location].value.data();
    }

    template<class T>
    inline void getattr(const float* const vert, int offset, T& attr) const {
        assert(sizeof(attr) % sizeof(float) == 0);
        attr = vert + offset;
    }

    template<class T>
    static void putvarying(floatstream& varying, const T& var);
    
    template<class T>
    static void getvaring(floatstream& varying, T& var, int& offset); 
};

template<class T>
void Shader::uniform(const T& unif, int location) {
    assert(sizeof(unif) % sizeof(float) == 0 && location >= 0);
    int num = sizeof(unif) / sizeof(float);   
    if(location >= uniforms.size()) uniforms.resize(location + 1);
    assert(uniforms[location].size == 0 || uniforms[location].size == num);
    uniforms[location].size = num;
    uniforms[location].value.resize(num);
    for(int i = 0; i < num; i++) {
        uniforms[location].value[i] = ((const float*)&unif)[i];
    }
}

template<class T>
void Shader::putvarying(floatstream& varying, const T& var) {
    assert(sizeof(var) % sizeof(float) == 0);
    int num = sizeof(var) / sizeof(float);
    for(int i = 0; i < num; i++) {
        varying.push_back(((const float*)&var)[i]);
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
