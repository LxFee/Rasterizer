#include "Shader.h"

void Shader::bindtexture(Texture *texture, int location) {
    assert(location >= 0);
    if(location >= textures.size()) textures.resize(location + 1, NULL);
    textures[location] = texture;
}
vec4 Shader::sample(int texture_location, float u, float v) const {
    if(texture_location < 0 || texture_location >= textures.size()) {
        return vec4(0.0f);
    }
    Texture* texture = textures[texture_location];
    return texture->sample(u, v);
}