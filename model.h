#pragma once
#include "mathbase.h"
#include <vector>
#include <string>


class Model {
public:
    Model(const std::string filename);
    int nverts() const;
    std::vector<vec3> verts{};
    std::vector<vec2> tex_coord{};
    std::vector<vec3> norms{};
};