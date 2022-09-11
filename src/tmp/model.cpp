#include "model.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

#include "mgl.h"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#endif

namespace {

    /**
     * @brief 从文件中读取纹理
     * 
     * @param image 文件路径
     * @return std::shared_ptr<Texture> 
     */
    int texture_from_file(std::string image) {
        int width, height, nrChannels;
        unsigned char *data = stbi_load(image.c_str(), &width, &height, &nrChannels, 0);
        if (!data) return -1;
        int texture_id = mgl_gen_texture_image(width, height, nrChannels, data);
        stbi_image_free(data);
        return texture_id;
    }

    static int get_id_from_filename(const std::string& filename) {
        int l = filename.size(), r = filename.size();
        for(int i = 0; i < filename.size(); i++) {
            if(filename[i] == '.') r = i;
            if(filename[i] == '_') l = i + 1;
        }
        int id = 0;
        for(; l < r; l++) {
            if(isdigit(filename[l])) {
                id = id * 10 + filename[l] - '0';
            }
        }
        return id;
    }

    static std::vector<std::string> get_texture_list(const std::string& filepath) {
        using namespace std;
        using namespace filesystem;
        path dir = path(filepath).remove_filename();
        string filename = path(filepath).filename().string();
        
        vector<string> list;
        if(!exists(dir) || !is_directory(dir)) {
            return list;
        }
        for(const auto& entry : directory_iterator(dir)) {
            if(!is_regular_file(entry.status())) continue;
            path current_path = entry.path();
            if( (current_path.has_extension()) && 
                (!current_path.extension().compare(".jpg") || !current_path.extension().compare(".png"))) {
                string current_path_filename = current_path.filename().string();
                bool flag = true;
                for(int i = 0; i < filename.size(); i++) {
                    if(filename[i] == '.') break;
                    if(filename[i] != current_path_filename[i]) {
                        flag = false;
                        break;
                    }
                }
                if(flag) 
                    list.push_back(current_path.string());
            }
        }
        return list;
    }
}

Model::Model(const std::string& filepath) : translation(0.0f), size(1.0f), rotation(0.0f) {
    load_from_file(filepath);
}

void Model::load_from_file(const std::string& filepath) {
    std::vector<vertex> verts{};
    using namespace std::filesystem;
    std::ifstream in;
    in.open(filepath, std::ifstream::in);
    if (in.fail()) return;
    

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
        if (!line.compare(0, 2, "f ")) {
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
                return;
            }
        }
    } 
    for(int i = 0; i < facet_vrt.size(); i += 3) {
        vec3 &a = vertexes[facet_vrt[i + 0]], &b = vertexes[facet_vrt[i + 1]], &c = vertexes[facet_vrt[i + 2]];
        vec3 f_normal = cross(b - a, c - a).normalized();
        for(int j = 0; j < 3; j++) {
            int f = facet_vrt[i + j];
            int n = facet_norm[i + j];
            vertex vert;
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
            vec3 edge1 = verts[ind + 1].position - verts[ind].position;
            vec3 edge2 = verts[ind + 2].position - verts[ind].position;
            vec2 deltaUV1 = verts[ind + 1].texcoord - verts[ind].texcoord;
            vec2 deltaUV2 = verts[ind + 2].texcoord - verts[ind].texcoord;
            float f = 1.0f / std::max((deltaUV1.x() * deltaUV2.y() - deltaUV2.x() * deltaUV1.y()), 1e-5f);
            vec3 tangent(   deltaUV2.y() * edge1.x() - deltaUV1.y() * edge2.x(),
                            deltaUV2.y() * edge1.y() - deltaUV1.y() * edge2.y(),
                            deltaUV2.y() * edge1.z() - deltaUV1.y() * edge2.z());
            tangent = (f * tangent).normalized();
            verts[ind].tangent = tangent;
            verts[ind + 1].tangent = tangent;
            verts[ind + 2].tangent = tangent;
        }
    }
    in.close();

    // bind data
    vbo = mgl_create_vbo(sizeof(vertex), verts.data(), verts.size());
    ebo = -1;
    mgl_vertex_attrib_pointer(vbo, 0, 3, 0); // position
    mgl_vertex_attrib_pointer(vbo, 1, 3, 3); // normal
    mgl_vertex_attrib_pointer(vbo, 2, 2, 6); // texcoord
    mgl_vertex_attrib_pointer(vbo, 3, 3, 8); // tangent
    // load texture
    auto texture_path_list = get_texture_list(filepath);
    for(auto texture_path : texture_path_list) {
        std::string filename = path(texture_path).filename().string();
        textures.push_back({get_id_from_filename(filename), texture_from_file(texture_path)});
    }
}

void Model::draw(Shader* shader) {
    for(int i = 0; i < textures.size(); i++) {
        mgl_active_texture(textures[i].second, textures[i].first);
    }
    shader->uniform(get_model_matrix(), 0);
    mgl_draw(vbo, ebo, shader);
}

// YXZ euler order 
void Model::set_rotation(vec3 rotation) {
    this->rotation = rotation;
}

void Model::set_translation(vec3 translation) {
    this->translation = translation;
}

void Model::set_size(vec3 size) {
    this->size = size;
}

mat4 Model::get_model_matrix() {
    mat4 R = euler_YXZ_rotate(rotation);
    mat4 T = translate(translation);
    mat4 S = scale(size);
    return T * S * R;
}