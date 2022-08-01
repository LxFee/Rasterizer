#include "Shader.h"

int Shader::bindtexture(Texture *texture, int location) {
    if(location >= (int)textures.size()) return -1;
    if(location < 0) {
        textures.push_back(texture);
        return (int)textures.size() - 1;
    }
    textures[location] = texture;
    return location;
}

vec4 Shader::sample(int texture_location, float u, float v) const {
    if(texture_location < 0 || texture_location >= textures.size()) {
        return vec4(0.0f);
    }
    Texture* texture = textures[texture_location];
    return texture->sample(u, v);
}