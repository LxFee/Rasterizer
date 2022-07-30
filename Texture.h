#pragma once
#include "mathbase.h"
#include <vector>
#include <string>


class Texture {
public:
    Texture(int w, int h);
    void get_size(int* w, int *h);
    void swap(std::vector<vec4> &rdata);
    vec4 sample(float u, float v);
    static Texture* readfromfile(std::string image);
private:
    int w, h;
    std::vector<vec4> data;
};