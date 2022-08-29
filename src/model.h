#pragma once
#include "mathbase.h"
#include "Shader.h"
#include <vector>
#include <string>
#include "Texture.h"

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
    void set_rotation(vec3 rotation); // YXZ euler order 
    void set_translation(vec3 translation);
    void set_size(vec3 size);
    mat4 get_model_matrix();
    std::vector<std::pair<int, Texture*>> textures;

private:
    vec3 translation, size, rotation;
    int vbo, ebo;
    std::vector<vertex> verts{};
    void load_from_file(const std::string& filepath);
};