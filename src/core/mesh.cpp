#include "mesh.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

namespace {
    vbo_t* convert_to_vbo(const std::vector<vertex_t>& vertexes) {
        vbo_t* vbo = new vbo_t(sizeof(vertex_t), vertexes.size());
        memcpy(vbo->data(), vertexes.data(), vbo->get_totol_size());
        return vbo;
    }

    vbo_t* load_from_file(const std::string& filename) {
        std::vector<vertex_t> verts;

        std::ifstream in;
        in.open(filename, std::ifstream::in);
        if (in.fail()) {
            std::cerr << "Error: can not open file" << std::endl;
            return NULL;
        }
        
        std::vector<vec3> vertexes;
        std::vector<vec3> normals;
        std::vector<vec2> uvs;
        std::vector<int> facet_vrt;
        std::vector<int> facet_norm;
        std::vector<int> facet_uv;
        
        std::string line;
        while (!in.eof()) {
            std::getline(in, line);
            std::istringstream iss(line.c_str());
            char trash;
            if (!line.compare(0, 2, "v ")) {
                iss >> trash;
                float x, y, z;
                iss >> x >> y >> z;
                vertexes.emplace_back(x, y, z);
            } else if (!line.compare(0, 3, "vn ")) {
                iss >> trash >> trash;
                float x, y, z;
                iss >> x >> y >> z;
                vec3 n(x, y, z);
                normals.emplace_back(n.normalized());
            } else if (!line.compare(0, 3, "vt ")) {
                iss >> trash >> trash;
                float u, v;
                iss >> u >> v;
                uvs.emplace_back(u, v);
            }
        }
        bool has_normal = !normals.empty();
        bool has_uv = !uvs.empty();
        if(!has_normal) {
            normals.resize(vertexes.size(), vec3(0.0f));
        }

        in.clear();
        in.seekg(0);
        while (!in.eof()) {
            std::getline(in, line);
            std::istringstream iss(line.c_str());
            char trash;
            if(!line.compare(0, 2, "f ")) {
                int f, t, n;
                iss >> trash;
                int cnt = 0;
                while(iss >> f) {
                    cnt++;
                    facet_vrt.push_back(--f);
                    iss >> trash;
                    if(has_uv) {
                        iss >> t;
                        facet_uv.push_back(--t);
                    }
                    if(has_normal) {
                        iss >> trash >> n;
                        facet_norm.push_back(--n);
                    } else {
                        facet_norm.push_back(f);
                    }
                }
                if(3 != cnt) {
                    std::cerr << "Error: the obj file is supposed to be triangulated" << std::endl;
                    in.close();
                    return NULL;
                }
            }
        } 

        for(int i = 0; i < facet_vrt.size(); i += 3) {
            vec3 &a = vertexes[facet_vrt[i + 0]], &b = vertexes[facet_vrt[i + 1]], &c = vertexes[facet_vrt[i + 2]];
            vec3 edge1 = b - a;
            vec3 edge2 = c - a;
            vec3 f_normal = cross(edge1, edge2).normalized();
            for(int j = 0; j < 3; j++) {
                int f = facet_vrt[i + j];
                int n = facet_norm[i + j];
                vertex_t vert;
                vert.position = vertexes[f];
                if(!has_normal) {
                    vert.normal = f_normal;
                } else {
                    vert.normal = normals[n].normalized();
                }
                if(!facet_uv.empty()) {
                    int t = facet_uv[i + j];
                    vert.texcoord = uvs[t];
                }
                verts.push_back(vert);
            }
            if(!facet_uv.empty()) {
                int ind = verts.size() - 3;
                vec2 deltaUV1 = verts[ind + 1].texcoord - verts[ind].texcoord;
                vec2 deltaUV2 = verts[ind + 2].texcoord - verts[ind].texcoord;
                vec3 tangent = vec3(deltaUV2.y() * edge1.x() - deltaUV1.y() * edge2.x(),
                                    deltaUV2.y() * edge1.y() - deltaUV1.y() * edge2.y(),
                                    deltaUV2.y() * edge1.z() - deltaUV1.y() * edge2.z()).normalized();
                vec3 bitangent = cross(f_normal, bitangent).normalized();
                verts[ind].tangent = tangent;
                verts[ind + 1].tangent = tangent;
                verts[ind + 2].tangent = tangent;
                verts[ind].bitangent = bitangent;
                verts[ind + 1].bitangent = bitangent;
                verts[ind + 2].bitangent = bitangent;
            }
        }
        in.close();
        return convert_to_vbo(verts);
    }
};




mesh_t::mesh_t(const std::string& filename) 
    : position(0.0f), rotation(0.0f), size(1.0f), vbo(load_from_file(filename)) {}

mesh_t::mesh_t(const std::vector<vertex_t>& vertexes)
    :position(0.0f), rotation(0.0f), size(1.0f), vbo(convert_to_vbo(vertexes)) {}

mesh_t::~mesh_t() {
    if(vbo) delete vbo;
}

mat4 mesh_t::get_model_matrix() const {
    mat4 R = euler_YXZ_rotate(rotation);
    mat4 T = translate(position);
    mat4 S = scale(size);
    return T * S * R;
}
    
void mesh_t::set_rotation(vec3 _rotation) {
    rotation = _rotation;
}

void mesh_t::set_position(vec3 _position) {
    position = _position;
}

void mesh_t::set_size(vec3 _size) {
    size = _size;
}

const vbo_t* mesh_t::get_vbo() const {
    return vbo;
}

