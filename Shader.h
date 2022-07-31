#pragma once
#include <vector>
#include "mathbase.h"
#include "Texture.h"
#include <cassert>
using floatstream = std::vector<float>;

class Shader {
public:
    virtual vec4 vertex_shader(int vbo, int index,floatstream & varying) const = 0;
    virtual vec4 fragment_shader(floatstream& varying) const = 0;
    int uniform(const float* el, int size, int location = -1);
    int bindtexture(Texture *texture, int location = -1);

protected:
    std::vector<float> uniforms;
    std::vector<int> uniforms_offset;
    std::vector<Texture*> textures;
    
    const float* getunif(int location, int check = -1) const ;
    const float* getattr(int vbo, int index, int location, int check = -1) const ;
    vec4 sample(int texture_location, float u, float v) const ;

    static void putvarying(floatstream& varying, const float *el, int size);
    static const float* getvaring(floatstream& varying, int size, int& offset);
};






