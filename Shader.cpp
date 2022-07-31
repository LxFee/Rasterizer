#include "Shader.h"
#include "mgl.h"

int Shader::uniform(const float* el, int size, int location) {
    if(location >= (int)uniforms_offset.size()) return -1;
    if(location < 0) {
        for(int i = 0; i < size; i++) uniforms.emplace_back(el[i]);
        int pre = 0;
        if(!uniforms_offset.empty()) pre = uniforms_offset.back();
        uniforms_offset.emplace_back(size + pre);
        return (int)uniforms_offset.size() - 1;
    }
    int pre = location > 0 ? uniforms_offset[location - 1] : 0;
    int cur = uniforms_offset[location];
    if(cur - pre != size) return -1;
    for(int i = 0; i < size; i++) uniforms[pre + i] = el[i];
    return location;
}

int Shader::bindtexture(Texture *texture, int location) {
    if(location >= (int)textures.size()) return -1;
    if(location < 0) {
        textures.push_back(texture);
        return (int)textures.size() - 1;
    }
    textures[location] = texture;
    return location;
}

void Shader::putvarying(floatstream& varying, const float *el, int size) {
    for(int i = 0; i < size; i++) varying.emplace_back(el[i]);
}

const float* Shader::getvaring(floatstream& varying, int size, int& offset) {
    int p = offset;
    offset += size;
    return varying.data() + p;    
}

const float* Shader::getunif(int location, int check) const {
    if(location < 0 || location >= (int)uniforms.size()) return NULL;
    int offset = 0;
    if(location) offset = uniforms_offset[location - 1];
    if(check >= 0 && check != uniforms_offset[location] - offset) return NULL;
    return uniforms.data() + offset;
}

const float* Shader::getattr(int vbo, int index, int location, int check) const {
    int size;
    float *vbop;
    if((vbop = mgl_query_vbo(vbo, index, location, &size)) == NULL) return NULL;
    if(check >= 0 && check != size) return NULL;
    return vbop;
}

vec4 Shader::sample(int texture_location, float u, float v) const {
    if(texture_location < 0 || texture_location >= textures.size()) {
        return vec4(0.0f);
    }
    Texture* texture = textures[texture_location];
    return texture->sample(u, v);
}