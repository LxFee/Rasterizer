#ifndef RASTERIZER_MESH_H_
#define RASTERIZER_MESH_H_

#include <vector>
#include <string>
#include "maths.h"
#include "graphics.h"


struct vertex_t {
    vec3 position;
    vec3 normal;
    vec2 texcoord;
    vec3 tangent;
    vec3 bitangent;
};

class mesh_t {
public:
    mesh_t(const std::string& filename);
    mesh_t(const std::vector<vertex_t>& vertexes);
    ~mesh_t();

    mat4 get_model_matrix() const;
    
    /** setter **/
    void set_rotation(vec3 rotation); 
    void set_translation(vec3 translation);
    void set_size(vec3 size);
private:
    vbo_t* vbo;
    void load_from_file(const std::string& filename);
    void convert_to_vbo(const std::vector<vertex_t>& vertexes);
};

#endif // RASTERIZER_MESH_H_