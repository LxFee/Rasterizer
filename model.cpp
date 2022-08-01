#include "model.h"
#include <iostream>
#include <sstream>
#include <fstream>

Model::Model(const std::string filename) {
    std::ifstream in;
    in.open(filename, std::ifstream::in);
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
            vec3 v;
            for (int i = 0; i < 3; i++) iss >> v.e[i];
            vertexes.push_back(v);
        } else if (!line.compare(0, 3, "vn ")) {
            iss >> trash >> trash;
            vec3 n;
            for (int i = 0; i < 3; i++) iss >> n.e[i];
            normals.push_back(n.normalized());
        } else if (!line.compare(0, 3, "vt ")) {
            iss >> trash >> trash;
            vec2 uv;
            for (int i = 0;i < 2; i++) iss >> uv.e[i];
            uvs.push_back(uv);
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
            if(!has_normal) {
                int ind = facet_vrt.size() - 3;
                vec3 &a = vertexes[facet_vrt[ind]], &b = vertexes[facet_vrt[ind + 1]], &c = vertexes[facet_vrt[ind + 2]];
                vec3 f_normal = cross(b - a, c - a).normalized();
                for(int i = ind; i < ind + 3; i++) {
                    normals[facet_vrt[i]] = normals[facet_vrt[i]] + f_normal;
                }
            }
        }
    } 
    for(int i = 0; i < facet_vrt.size(); i++) {
        int f = facet_vrt[i];
        int n = facet_norm[i];
        verts.push_back(vertexes[f]);
        norms.push_back(normals[n].normalized());
        if(!facet_uv.empty()) {
            int t = facet_uv[i];
            tex_coord.push_back(uvs[t]);
        }
    }
    in.close();
    // std::cerr << "# v# " << nverts() << std::endl;
}

int Model::nverts() const {
    return verts.size();
}

