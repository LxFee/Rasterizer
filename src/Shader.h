#ifndef RASTERIZER_SHADER_H_
#define RASTERIZER_SHADER_H_

#include <vector>
#include <cassert>
#include <memory>

#include "mathbase.h"
#include "Texture.h"

using floatstream = std::vector<float>;
extern float* mgl_query_vbo(int, int, int, int*);

struct uniform_element {
    uniform_element() : size(0) {}
    int size; // 单位：float（4字节）
    floatstream value;
};

class Shader {
public:
    virtual vec4 vertex_shader(const float* const vert, const std::vector<int>& offset, floatstream & varying) const = 0;
    virtual vec4 fragment_shader(floatstream& varying) const = 0;
    
    template<class T>
    inline void uniform(const T& unif, int location) {
        assert( (sizeof(unif) % sizeof(float) == 0));
        int num = sizeof(unif) / sizeof(float);   
        if(location >= uniforms.size()) 
            uniforms.resize(location + 1);
        assert(uniforms[location].size == 0 || uniforms[location].size == num);
        uniforms[location].size = num;
        uniforms[location].value.resize(num);
        for(int i = 0; i < num; i++) {
            uniforms[location].value[i] = ((const float*)&unif)[i];
        }
    }

    inline void bindtexture(std::shared_ptr<Texture> texture, int location) {
        assert(location >= 0);
        if(location >= textures.size()) textures.resize(location + 1, NULL);
        textures[location] = texture;
    }

    virtual ~Shader() {}

protected:
    std::vector<uniform_element> uniforms;
    std::vector<std::shared_ptr<Texture>> textures;

    vec4 sample(int texture_location, float u, float v) const {
        if(texture_location < 0 || texture_location >= textures.size()) {
            return vec4(0.0f);
        }
        auto &texture = textures[texture_location];
        return texture->sample(u, v);
    }

    template<class T>
    inline void getunif(int location, T& unif) const {
        assert( (sizeof(unif) % sizeof(float) == 0) && 
                (location >= 0 && location < (int)uniforms.size()) && 
                (uniforms[location].size == sizeof(unif) / sizeof(float)));
        unif = uniforms[location].value.data();
    }

    template<class T>
    inline void getattr(const float* const vert, int offset, T& attr) const {
        assert(sizeof(attr) % sizeof(float) == 0);
        attr = vert + offset;
    }

    template<class T>
    static inline void putvarying(floatstream& varying, const T& var) {
        assert(sizeof(var) % sizeof(float) == 0);
        for(int i = 0; i < sizeof(var) / sizeof(float); i++) {
            varying.push_back(((const float*)&var)[i]);
        }
    }
    
    template<class T>
    static inline void getvaring(floatstream& varying, T& var, int& offset) {
        assert( (sizeof(var) % sizeof(float) == 0) &&
                (offset + sizeof(var) / sizeof(float) <= varying.size()));
        var = varying.data() + offset;
        offset += sizeof(var) / sizeof(float);
    }
};


#endif // RASTERIZER_SHADER_H_