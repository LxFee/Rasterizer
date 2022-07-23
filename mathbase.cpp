#include "mathbase.h"
#include <cmath>

vec4::vec4() {
    e[0] = e[1] = e[2] = e[3] = 0.0f;
}
vec4::vec4(float k) {
    e[0] = e[1] = e[2] = e[3] = k;
}
vec4::vec4(float x, float y, float z, float w) {
    e[0] = x;
    e[1] = y;
    e[2] = z;
    e[3] = w;
}

float vec4::length_squared() const {
    return e[0] * e[0] + e[1] * e[1] + e[2] * e[2] + e[3] * e[3];
}

float vec4::length() const {
    return sqrt(length_squared());
}

vec4 vec4::normalized() const {
    return *this / length();
}

vec4 vec4::operator + (const vec4& rhs) const {
    return vec4(e[0] + rhs.e[0], e[1] + rhs.e[1], e[2] + rhs.e[2], e[3] + rhs.e[3]);
}

vec4 vec4::operator - () const {
    return vec4(-e[0], -e[1], -e[2], -e[3]);
}

vec4 vec4::operator - (const vec4& rhs) const {
    return *this + (-rhs);
}

vec4 vec4::operator * (const vec4& rhs) const {
    return vec4(e[0] * rhs.e[0], e[1] * rhs.e[1], e[2] * rhs.e[2], e[3] * rhs.e[3]);
}

vec4 vec4::operator / (float k) const {
    return vec4(e[0] / k, e[1] / k, e[2] / k, e[3] / k);
}

float vec4::x() const {return e[0];}
float vec4::y() const {return e[1];} 
float vec4::z() const {return e[2];}
float vec4::w() const {return e[3];}

float vec4::r() const  {return e[0];} 
float vec4::g() const  {return e[1];} 
float vec4::b() const  {return e[2];}
float vec4::a() const  {return e[3];}

float clamp(float x, float mi, float mx) {
    if(x < mi) return mi;
    if(x > mx) return mx;
    return x;
}

int packRGBA8888(vec4 col) {
    int r = static_cast<int>(256 * clamp(col.r(), 0.0, 0.9999));
    int g = static_cast<int>(256 * clamp(col.g(), 0.0, 0.9999));
    int b = static_cast<int>(256 * clamp(col.b(), 0.0, 0.9999));
    int a = static_cast<int>(256 * clamp(col.a(), 0.0, 0.9999));
    return a + (r << 8) + (g << 16) + (b << 24);
}