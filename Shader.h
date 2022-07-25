#pragma once
#include <vector>
#include "mathbase.h"
#include <cassert>
using floatstream = std::vector<float>;

class Shader {
public:
    virtual vec4 vertex_shader(int vbo, int index,floatstream & varying) const = 0;
    virtual vec4 fragment_shader(floatstream& varying) const = 0;
    uniform(float* el, int size, int location = -1);

private:
    std::vector<float> uniforms;
    std::vector<int> uniforms_offset;
    
    float* getunif(int location, int check = -1);
    float* getattr(int vbo, int index, int location, int check = -1);

    void putvarying(floatstream& varying, float *el, int size);
    float* getvaring(floatstream& varying, int size, int& offset);
};






