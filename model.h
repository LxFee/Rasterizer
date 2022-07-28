#pragma once
#include "mathbase.h"

struct Triangle {
    Triangle();
    Triangle(vec2 a, vec2 b, vec2 c);
    vec2 points[3];
};

void calc_tr_coords(const Triangle& tr, const vec2& point, float& alpha, float &beta, float &gamma);

