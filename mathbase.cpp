#include "mathbase.h"
#include <cmath>

/********************** vec4 **********************/
vec4::vec4() {
    e[0] = e[1] = e[2] = e[3] = 0.0f;
}
vec4::vec4(float k) {
    e[0] = e[1] = e[2] = e[3] = k;
}
vec4::vec4(const float *ep) {
    e[0] = ep[0];
    e[1] = ep[1];
    e[2] = ep[2];
    e[3] = ep[3];
}
vec4::vec4(float x, float y, float z, float w) {
    e[0] = x;
    e[1] = y;
    e[2] = z;
    e[3] = w;
}

vec4::vec4(const vec3& v, float w) {
    e[0] = v.e[0];
    e[1] = v.e[1];
    e[2] = v.e[2];
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

vec4 operator*(float k, const vec4& rhs) {
    return vec4(k * rhs.e[0], k * rhs.e[1], k * rhs.e[2], k * rhs.e[3]);
}

vec4 operator*(const vec4& lhs, float k) {
    return vec4(k * lhs.e[0], k * lhs.e[1], k * lhs.e[2], k * lhs.e[3]);
}


/********************** vec3 **********************/
vec3::vec3() {
    e[0] = e[1] = e[2] = 0.0f;
}
vec3::vec3(float k) {
    e[0] = e[1] = e[2] = k;
}

vec3::vec3(const float *ep) {
    e[0] = ep[0];
    e[1] = ep[1];
    e[2] = ep[2];
}
vec3::vec3(float x, float y, float z) {
    e[0] = x;
    e[1] = y;
    e[2] = z;
}

float vec3::length_squared() const {
    return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
}

float vec3::length() const {
    return sqrt(length_squared());
}

vec3 vec3::normalized() const {
    return *this / length();
}

vec3 vec3::operator + (const vec3& rhs) const {
    return vec3(e[0] + rhs.e[0], e[1] + rhs.e[1], e[2] + rhs.e[2]);
}

vec3 vec3::operator - () const {
    return vec3(-e[0], -e[1], -e[2]);
}

vec3 vec3::operator - (const vec3& rhs) const {
    return *this + (-rhs);
}

vec3 vec3::operator * (const vec3& rhs) const {
    return vec3(e[0] * rhs.e[0], e[1] * rhs.e[1], e[2] * rhs.e[2]);
}

vec3 vec3::operator / (float k) const {
    return vec3(e[0] / k, e[1] / k, e[2] / k);
}

float vec3::x() const {return e[0];}
float vec3::y() const {return e[1];} 
float vec3::z() const {return e[2];}

float vec3::r() const  {return e[0];} 
float vec3::g() const  {return e[1];} 
float vec3::b() const  {return e[2];}

vec3 cross(const vec3 &lhs, const vec3 &rhs) {
    return vec3(lhs.y() * rhs.z() - rhs.y() * lhs.z(),
                lhs.z() * rhs.x() - rhs.z() * lhs.x(),
                lhs.x() * rhs.y() - rhs.x() * lhs.y());
}

float dot(const vec3 &lhs, const vec3 &rhs) {
    return lhs.x() * rhs.x() + lhs.y() * rhs.y() + lhs.z() * rhs.z();
}

vec3 operator*(float k, const vec3& rhs) {
    return vec3(k * rhs.e[0], k * rhs.e[1], k * rhs.e[2]);
}

vec3 operator*(const vec3& lhs, float k) {
    return vec3(k * lhs.e[0], k * lhs.e[1], k * lhs.e[2]);
}

/********************** vec2 **********************/
vec2::vec2() {
    e[0] = e[1] = 0.0f;
}
vec2::vec2(float k) {
    e[0] = e[1] = k;
}

vec2::vec2(const float *ep) {
    e[0] = ep[0];
    e[1] = ep[1];
}
vec2::vec2(float x, float y) {
    e[0] = x;
    e[1] = y;
}

float vec2::length_squared() const {
    return e[0] * e[0] + e[1] * e[1];
}

float vec2::length() const {
    return sqrt(length_squared());
}

vec2 vec2::normalized() const {
    return *this / length();
}

vec2 vec2::operator + (const vec2& rhs) const {
    return vec2(e[0] + rhs.e[0], e[1] + rhs.e[1]);
}

vec2 vec2::operator - () const {
    return vec2(-e[0], -e[1]);
}

vec2 vec2::operator - (const vec2& rhs) const {
    return *this + (-rhs);
}

vec2 vec2::operator * (const vec2& rhs) const {
    return vec2(e[0] * rhs.e[0], e[1] * rhs.e[1]);
}

vec2 vec2::operator / (float k) const {
    return vec2(e[0] / k, e[1] / k);
}

float vec2::x() const {return e[0];}
float vec2::y() const {return e[1];}
float vec2::u() const {return e[0];} 
float vec2::v() const {return e[1];}

vec2 operator*(float k, const vec2& rhs) {
    return vec2(k * rhs.e[0], k * rhs.e[1]);
}

vec2 operator*(const vec2& lhs, float k) {
    return vec2(k * lhs.e[0], k * lhs.e[1]);
}

float cross(const vec2 &lhs, const vec2 &rhs) {
    return lhs.x() * rhs.y() - rhs.x() * lhs.y();
}

/********************** mat4 **********************/
mat4::mat4(float k) {
    for(int i = 0; i < 16; i++) e[i] = 0.0f;
    for(int i = 0; i < 4; i++) e[i * 4 + i] = k;
}

mat4::mat4(const float *ep) {
    for(int i = 0; i < 16; i++) e[i] = ep[i];
}

mat4::mat4( float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33) {
    e[0] = m00;
    e[1] = m01;
    e[2] = m02;
    e[3] = m03;
    e[4] = m10;
    e[5] = m11;
    e[6] = m12;
    e[7] = m13;
    e[8] = m20;
    e[9] = m21;
    e[10] = m22;
    e[11] = m23;
    e[12] = m30;
    e[13] = m31;
    e[14] = m32;
    e[15] = m33;
}

mat4::mat4(vec4 v0, vec4 v1, vec4 v2, vec4 v3) {
    for(int i = 0; i < 4; i++) e[i * 4 + 0] = v0.e[i];
    for(int i = 0; i < 4; i++) e[i * 4 + 1] = v1.e[i];
    for(int i = 0; i < 4; i++) e[i * 4 + 2] = v2.e[i];
    for(int i = 0; i < 4; i++) e[i * 4 + 3] = v3.e[i];
}

vec4 mat4::operator * (const vec4& rhs) const {
    vec4 res(0.0f);
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            res.e[i] += rhs.e[j] * e[i * 4 + j];
        }
    }
    return res;
}
mat4 mat4::operator + (const mat4& rhs) const {
    mat4 res(0.0f);
    for(int i = 0; i < 16; i++) res.e[i] = e[i] + rhs.e[i];
    return res;
}

mat4 mat4::operator * (const mat4& rhs) const {
    mat4 res(0.0f);
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            for(int k = 0; k < 4; k++) {
                res.e[i * 4 + j] += e[i * 4 + k] * rhs.e[k * 4 + j];
            }
        }
    }
    return res;
}

mat4 mat4::T() const {
    mat4 res;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            res.e[i * 4 + j] = e[j * 4 + i];
        }
    }
    return res;
}

mat4 mat4::operator * (float k) const {
    mat4 res(*this);
    for(int i = 0; i < 16; i++) res.e[i] *= k;
    return res;
}




/********************** Tool Functions **********************/

float clamp(float x, float mi, float mx) {
    if(x < mi) return mi;
    if(x > mx) return mx;
    return x;
}

mat4 translate(vec3 tr) {
    return mat4(1.0f, 0.0f, 0.0f, tr.x(),
                0.0f, 1.0f, 0.0f, tr.y(),
                0.0f, 0.0f, 1.0f, tr.z(),
                0.0f, 0.0f, 0.0f, 1.0f);
}

mat4 scale(vec3 sc) {
    return mat4(sc.x(), 0.0f, 0.0f, 0.0f,
                0.0f, sc.y(), 0.0f, 0.0f,
                0.0f, 0.0f, sc.z(), 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
}

mat4 ortho(float l, float r, float b, float t, float n, float f) {
    return  mat4(2.0f / (r - l), 0.0f, 0.0f, (l + r) / (l - r),
                 0.0f, 2.0f / (t - b), 0.0f, (b + t) / (b - t),
                 0.0f, 0.0f, 2.0f / (n - f), (n + f) / (n - f),
                 0.0f, 0.0f, 0.0f, 1.0f);
}

// fov: degree
mat4 perspective(float n, float f, float fov, float aspect) {
    float t = tan(radian(fov) / 2.0f) * n;
    float r = aspect * t;
    return  mat4(   n / r, 0.0f, 0.0f, 0.0f,
                    0.0f, n / t, 0.0f, 0.0f,
                    0.0f, 0.0f, (n + f) / (n - f), 2.0f * n * f / (n - f),
                    0.0f, 0.0f, -1.0f, 0.0f);
}

mat4 rotate(vec3 axis, float angle) {
    angle = radian(angle);
    mat4 res(1.0f);
    mat4 N( 0.0f, -axis.z(), axis.y(), 0.0f,
            axis.z(), 0.0f, -axis.x(), 0.0f,
            -axis.y(), axis.x(), 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f);
    mat4 maxis( axis.x() * axis.x(), axis.x() * axis.y(), axis.x() * axis.z(), 0.0f,
                axis.y() * axis.x(), axis.y() * axis.y(), axis.y() * axis.z(), 0.0f,
                axis.z() * axis.x(), axis.z() * axis.y(), axis.z() * axis.z(), 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f);

    res = res * cos(angle) + maxis * (1 - cos(angle)) + N * sin(angle);
    res.e[15] = 1.0f;
    return res;    
}

