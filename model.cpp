#include "model.h"


Triangle::Triangle() {}
Triangle::Triangle(vec2 a, vec2 b, vec2 c) {
    points[0] = a;
    points[1] = b;
    points[2] = c;
}

void calc_tr_coords(const Triangle& tr, const vec2& point, float& alpha, float &beta, float &gamma) {
    vec2 ca = tr.points[0] - tr.points[2];
    float cross_ac = tr.points[0].x() * tr.points[2].y() - tr.points[0].y() * tr.points[2].x();

    vec2 ba = tr.points[0] - tr.points[1];
    float cross_ab = tr.points[0].x() * tr.points[1].y() - tr.points[0].y() * tr.points[1].x();

    beta = (ca.y() * point.x() - ca.x() * point.y() + cross_ac) / (ca.y() * tr.points[1].x() - ca.x() * tr.points[1].y() + cross_ac);
    gamma = (ba.y() * point.x() - ba.x() * point.y() + cross_ab) / (ba.y() * tr.points[2].x() - ba.x() * tr.points[2].y() + cross_ab);
    alpha = 1.0f - beta - gamma;
}
