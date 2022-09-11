#include "shader.h"

shader_t::shader_t(int sizeof_uniform, int sizeof_varyings) 
    : sizeof_uniform(sizeof_uniform), sizeof_varyings(sizeof_varyings), enable_blend(false) {
    uniform = new char[sizeof_uniform];
}

shader_t::~shader_t() {
    delete [] uniform;
}


int shader_t::get_sizeof_uniform() const {
    return sizeof_uniform;
}
int shader_t::get_sizeof_varyings() const {
    return sizeof_varyings;
}