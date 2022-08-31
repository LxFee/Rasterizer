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
    /**
     * @brief 顶点着色器
     * 
     * @param vert 当前顶点的属性，请使用getattr方法获取里面的内容
     * @param offset 当前顶点的格式，offset[i]代表第i号属性的偏移，请配合使用getattr获得相应的属性值
     * @param varying 需要传入片元着色器（fragment_shader）的量，请配合putvarying使用
     * @return vec4 经过顶点着色器处理后的顶点位置
     */
    virtual vec4 vertex_shader(const float* const vert, const std::vector<int>& offset, floatstream & varying) const = 0;
    
    /**
     * @brief 片元着色器
     * 
     * @param varying 从顶点着色器过来，经过插值的量。请配合getvarying使用
     * @return vec4 最终颜色
     */
    virtual vec4 fragment_shader(const floatstream& varying) const = 0;
    
    /**
     * @brief 向着色器传入uniform变量
     * 第一次传入可以是任意mathbase中的类型的值；否则如果传入到相同location时，类型必须一致，并将覆盖掉原有的值。
     * 
     * @tparam T 
     * @param unif 传入的量
     * @param location 传入的位置
     */
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

    /**
     * @brief 绑定纹理
     * 
     * @param texture 纹理指针
     * @param location 绑定的位置
     */
    inline void bindtexture(std::shared_ptr<Texture> texture, int location) {
        assert(location >= 0);
        if(location >= textures.size()) textures.resize(location + 1, NULL);
        textures[location] = texture;
    }

    virtual ~Shader() {}

protected:
    std::vector<uniform_element> uniforms;
    std::vector<std::shared_ptr<Texture>> textures;

    /**
     * @brief 在着色器中使用。采样纹理
     * 
     * @param texture_location 纹理编号，在bindtexture()中的设置的
     * @param u 
     * @param v 
     * @return vec4 
     */
    vec4 sample(int texture_location, float u, float v) const {
        if(texture_location < 0 || texture_location >= textures.size()) {
            return vec4(0.0f);
        }
        auto &texture = textures[texture_location];
        return texture->sample(u, v);
    }

    /**
     * @brief 在着色器中使用。获得uniform变量
     * 当uniform中记录的变量类型和传入的unif类型不符时，会触发assert
     * 
     * @tparam T 
     * @param location uniform变量编号，在uniform()中设置的
     * @param unif 用于存放得到的uniform变量
     */
    template<class T>
    inline void getunif(int location, T& unif) const {
        assert( (sizeof(unif) % sizeof(float) == 0) && 
                (location >= 0 && location < (int)uniforms.size()) && 
                (uniforms[location].size == sizeof(unif) / sizeof(float)));
        unif = T(uniforms[location].value.data());
    }

    /**
     * @brief 在顶点着色器中使用。获得顶点属性
     * 
     * @tparam T 
     * @param vert 顶点属性数据。从顶点着色器的参数得到。
     * @param offset 顶点属性的偏移量。第i个属性的偏移量可以从顶点着色器参数offset[i]得到
     * @param attr 用于存放得到的顶点属性
     */
    template<class T>
    inline void getattr(const float* const vert, int offset, T& attr) const {
        assert(sizeof(attr) % sizeof(float) == 0);
        attr = T(vert + offset);
    }

    /**
     * @brief 在顶点着色器中使用，放入需要插值并传递到片元着色器的量
     * 注意放入顺序和取出顺序要相同
     * 
     * @tparam T 
     * @param varying 可以从顶点着色器的参数中得到
     * @param var 需要传入到片元着色器的量
     */
    template<class T>
    static inline void putvarying(floatstream& varying, const T& var) {
        assert(sizeof(var) % sizeof(float) == 0);
        for(int i = 0; i < sizeof(var) / sizeof(float); i++) {
            varying.push_back(((const float*)&var)[i]);
        }
    }
    
    /**
     * @brief 在片元着色器中使用，得到从顶点着色器来的，经过插值的量
     * 注意取出顺序要和放入顺序相同
     * 
     * @tparam T 
     * @param varying 可以从片元着色器的参数中得到
     * @param var 用于存放得到的变量
     * @param offset 辅助使用，第一个放入的量offset=0。getvaring会自动帮你累加offset，因此在取下一个变量的重复传入同一个offset即可。
     */
    template<class T>
    static inline void getvaring(const floatstream& varying, T& var, int& offset) {
        assert( (sizeof(var) % sizeof(float) == 0) &&
                (offset + sizeof(var) / sizeof(float) <= varying.size()));
        var = T(varying.data() + offset);
        offset += sizeof(var) / sizeof(float);
    }
};


#endif // RASTERIZER_SHADER_H_