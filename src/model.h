#ifndef RASTERIZER_MODEL_H_
#define RASTERIZER_MODEL_H_

#include <vector>
#include <string>
#include <memory>

#include "shader.h"
#include "Texture.h"
#include "mathbase.h"

struct vertex {
    vec3 position;
    vec3 normal;
    vec2 texcoord;
    vec3 tangent;
};

class Model {
public:
    /**
     * @brief 从文件中读取物体，并自动绑定vbo，ebo。如果同一文件夹下有同名带编号的图片文件（png或jpg），会读入作为该物体的纹理。
     * 纹理名格式：模型名_备注_编号.jpg(png)
     * 纹理用于确定传入shader第几号texture，从0开始。
     * 
     * @param filepath 模型文件路径（只支持obj）
     */
    Model(const std::string& filepath);
    
    /**
     * @brief 调用mgl_draw绘制物体
     * 
     * @param shader 物体变换矩阵传入到location=0的uniform变量中；将纹理按顺序传入shader中
     */
    void draw(Shader* shader);
    
    int nverts() const;

    /**
     * @brief YXZ规则欧拉角旋转
     * 
     * @param rotation 
     */
    void set_rotation(vec3 rotation); 
    
    void set_translation(vec3 translation);
    
    void set_size(vec3 size);
    
    mat4 get_model_matrix();
    
    std::vector<std::pair<int, std::shared_ptr<Texture>>> textures;
private:
    vec3 translation, size, rotation;
    int vbo, ebo;
    std::vector<vertex> verts{};
    void load_from_file(const std::string& filepath);
};

#endif // RASTERIZER_MODEL_H_