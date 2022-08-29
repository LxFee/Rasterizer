#ifndef RASTERIZER_MODEL_H_
#define RASTERIZER_MODEL_H_

#include <vector>
#include <string>
#include <memory>

#include "Shader.h"
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
    Model(const std::string& filepath);
    
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