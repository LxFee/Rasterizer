#pragma once
#include "mathbase.h"
#include <vector>
#include <string>


class Texture {
public:
    enum SURROUND {REPEAT, FILLED};
    enum INTERPOLATION {BILINEAR, NEAREST};

    Texture(int w, int h);
    void swap(std::vector<vec4> &rdata);
    vec4 sample(float u, float v);
    static Texture* readfromfile(std::string image);
    void query(int* w, int *h, int *surr, int *intp);
    void set_filled_color(vec4 color);
    void set_srround(SURROUND surr);
    void set_interpolation(INTERPOLATION intp);

    static Texture from_image(std::string image);
private:
    int w, h;
    std::vector<vec4> data;
    SURROUND surround = FILLED;
    INTERPOLATION interpolation = BILINEAR;
    vec4 filled_color;
};