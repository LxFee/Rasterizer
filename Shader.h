#pragma once
#include <vector>
#include "mathbase.h"
#include <cassert>
using floatstream = std::vector<float>;

class Shader {
public:
    virtual vec4 vertex_shader(int vbo, int index,floatstream & varying) const = 0;
    virtual vec4 fragment_shader(floatstream& varying) const = 0;
    uniform(const float* el, int size, int location = -1);

protected:
    std::vector<float> uniforms;
    std::vector<int> uniforms_offset;
    
    const float* getunif(int location, int check = -1) const ;
    const float* getattr(int vbo, int index, int location, int check = -1) const ;

    static void putvarying(floatstream& varying, const float *el, int size);
    static const float* getvaring(floatstream& varying, int size, int& offset);
};






