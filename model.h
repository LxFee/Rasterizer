#pragma once
#include "mathbase.h"
#include "Shader.h"
#include <vector>
#include <string>
#include "Texture.h"

class Model {
public:
    Model(const std::string filepath);
    void draw(Shader* shader);
    int nverts() const;
    void set_rotation(vec3 rotation); // YXZ euler order 
    void set_translation(vec3 translation);
    void set_size(vec3 size);
    mat4 get_model_matrix();
    std::vector<std::pair<int, Texture*>> textures;

private:
    vec3 translation, size;
    vec3 rotation;
    int vbo, ebo;
    std::vector<vec3> verts{};
    std::vector<vec2> tex_coord{};
    std::vector<vec3> norms{};
    std::vector<vec3> tangents{};
    std::vector<int> indexs;

};