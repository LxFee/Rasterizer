#pragma once
#include "mathbase.h"
#include <vector>


class Texture {
public:
    Texture(int w, int h);
    void get_size(int* w, int *h);
    void swap(std::vector<vec4> &rdata); 
private:
    int w, h;
    std::vector<vec4> data;
};