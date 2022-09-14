#include "shader.h"

#include <cstring>

shader_t::shader_t(int sizeof_varyings)
    : uniforms(NULL), sizeof_varyings(sizeof_varyings) {}

shader_t::~shader_t() {}

int shader_t::get_sizeof_varyings() const { return sizeof_varyings; }

void shader_t::bind_uniform(void *uniform_data) { uniforms = uniform_data; }