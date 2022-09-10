#ifndef RASTERIZER_SHADER_H_
#define RASTERIZER_SHADER_H_

#include "maths.h"

class shader_t {
public:
    shader_t(int sizeof_uniform, int sizeof_varyings);
    virtual ~shader_t();

    virtual const vec4 vertex_shader(const void *attribs, void *varyings) = 0;
    virtual const vec4 fragment_shader(const void *varyings, bool& discard) = 0;

    int get_sizeof_uniform() const ;
    int get_sizeof_varyings() const ;

protected:
    void *uniform;
    int sizeof_uniform;
    int sizeof_varyings;
    bool enable_blend;
};
#endif // RASTERIZER_SHADER_H_