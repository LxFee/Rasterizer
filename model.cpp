#include "model.h"
#include <iostream>
#include <sstream>
#include <fstream>

Model::Model(const std::string filename) {
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail()) return;
    std::vector<vec3> normals;
    std::vector<vec2> uvs;
    std::vector<int> normal_count;
    
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            vec3 v;
            for (int i = 0; i < 3; i++) iss >> v.e[i];
            verts.push_back(v);
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
    in.clear();
    in.seekg(0);
    norms.resize(verts.size(), vec3(0.0f));
    tex_coord.resize(verts.size(), vec2(0.0f));
    normal_count.resize(verts.size(), 0);

    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "f ")) {
            int f,t,n;
            iss >> trash;
            int cnt = 0;
            if(has_normal) {
                while (iss >> f >> trash >> t >> trash >> n) {
                    facet_vrt.push_back(--f);
                    norms[f] = normals[--n];
                    tex_coord[f] = uvs[--t];
                    cnt++;
                }
                if (3!=cnt) {
                    std::cerr << "Error: the obj file is supposed to be triangulated" << std::endl;
                    in.close();
                    return;
                }
            } else {
                while (iss >> f >> trash >> t) {
                    facet_vrt.push_back(--f);
                    tex_coord[f] = uvs[--t];
                    cnt++;
                }
                int ind = facet_vrt.size() - 3;
                vec3 fnormal = cross(verts[facet_vrt[ind + 1]] - verts[facet_vrt[ind]], verts[facet_vrt[ind + 2]] - verts[facet_vrt[ind]]).normalized();
                for(int i = ind; i < ind + 3; i++) {
                    norms[facet_vrt[i]] = norms[facet_vrt[i]] + fnormal;
                    normal_count[facet_vrt[i]]++;
                }
                if (3!=cnt) {
                    std::cerr << "Error: the obj file is supposed to be triangulated" << std::endl;
                    in.close();
                    return;
                }
            }
        }
    } 
    if(!has_normal) {
        for(int i = 0; i < norms.size(); i++) {
            if(normal_count[i]) {
                norms[i] = norms[i].normalized();
            }
        }
    }
    in.close();
    std::cout << has_normal << std::endl;
    // std::cerr << "# v# " << nverts() << " f# "  << nfaces() << " vt# " << tex_coord.size() << " vn# " << norms.size() << std::endl;
}

int Model::nverts() const {
    return verts.size();
}

int Model::nfaces() const {
    return facet_vrt.size()/3;
}

