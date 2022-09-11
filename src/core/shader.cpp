#include "shader.h"
#include <cstring>

shader_t::shader_t(int sizeof_uniforms, int sizeof_varyings) 
    : sizeof_uniforms(sizeof_uniforms), sizeof_varyings(sizeof_varyings), enable_blend(false) {
    uniforms = new char[sizeof_uniforms];
}

shader_t::~shader_t() {
    delete [] uniforms;
}


int shader_t::get_sizeof_uniforms() const {
    return sizeof_uniforms;
}
int shader_t::get_sizeof_varyings() const {
    return sizeof_varyings;
}

void shader_t::set_uniform(void *uniform_data) {
    if(!uniform_data) return ;
    memcpy(uniforms, uniform_data, sizeof_uniforms);
}