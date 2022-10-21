#include "core/maths.h"

#include <cassert>
#include <cstdio>

/********************** vec4 **********************/

vec4::vec4() { e[0] = e[1] = e[2] = e[3] = 0; }

vec4::vec4(float k) { e[0] = e[1] = e[2] = e[3] = k; }

vec4::vec4(const float* data) {
    e[0] = data[0];
    e[1] = data[1];
    e[2] = data[2];
    e[3] = data[3];
}
vec4::vec4(float x, float y, float z, float w) {
    e[0] = x;
    e[1] = y;
    e[2] = z;
    e[3] = w;
}

vec4::vec4(vec3 v, float w) {
    e[0] = v.x();
    e[1] = v.y();
    e[2] = v.z();
    e[3] = w;
}

float vec4::length_squared() const {
    return e[0] * e[0] + e[1] * e[1] + e[2] * e[2] + e[3] * e[3];
}

float vec4::length() const { return sqrt(length_squared()); }

const vec4 vec4::normalized() const { return *this / length(); }

const vec4 vec4::operator+(const vec4& rhs) const {
    return vec4(e[0] + rhs.e[0], e[1] + rhs.e[1], e[2] + rhs.e[2],
                e[3] + rhs.e[3]);
}

const vec4 vec4::operator-(const vec4& rhs) const { return *this + (-rhs); }

const vec4 vec4::operator*(const vec4& rhs) const {
    return vec4(e[0] * rhs.e[0], e[1] * rhs.e[1], e[2] * rhs.e[2],
                e[3] * rhs.e[3]);
}

const vec4 vec4::operator/(const vec4& rhs) const {
    return vec4(e[0] / rhs.e[0], e[1] / rhs.e[1], e[2] / rhs.e[2],
                e[3] / rhs.e[3]);
}

const vec4 vec4::operator-() const { return vec4(-e[0], -e[1], -e[2], -e[3]); }

float* vec4::data() { return e; }

const float* vec4::data() const { return e; }

const float vec4::dot(const vec4& rhs) const {
    return e[0] * rhs.e[0] + e[1] * rhs.e[1] + e[2] * rhs.e[2] +
           e[3] * rhs.e[3];
}

float& vec4::x() { return e[0]; }
float& vec4::y() { return e[1]; }
float& vec4::z() { return e[2]; }
float& vec4::w() { return e[3]; }
const float& vec4::x() const { return e[0]; }
const float& vec4::y() const { return e[1]; }
const float& vec4::z() const { return e[2]; }
const float& vec4::w() const { return e[3]; }

float& vec4::r() { return e[0]; }
float& vec4::g() { return e[1]; }
float& vec4::b() { return e[2]; }
float& vec4::a() { return e[3]; }
const float& vec4::r() const { return e[0]; }
const float& vec4::g() const { return e[1]; }
const float& vec4::b() const { return e[2]; }
const float& vec4::a() const { return e[3]; }

/********************** vec3 **********************/
vec3::vec3() { e[0] = e[1] = e[2] = 0; }

vec3::vec3(float k) { e[0] = e[1] = e[2] = k; }

vec3::vec3(const float* data) {
    e[0] = data[0];
    e[1] = data[1];
    e[2] = data[2];
}

vec3::vec3(float x, float y, float z) {
    e[0] = x;
    e[1] = y;
    e[2] = z;
}

float vec3::length_squared() const {
    return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
}

float vec3::length() const { return sqrt(length_squared()); }

const vec3 vec3::normalized() const { return *this / length(); }

const vec3 vec3::operator+(const vec3& rhs) const {
    return vec3(e[0] + rhs.e[0], e[1] + rhs.e[1], e[2] + rhs.e[2]);
}

const vec3 vec3::operator-(const vec3& rhs) const { return *this + (-rhs); }

const vec3 vec3::operator*(const vec3& rhs) const {
    return vec3(e[0] * rhs.e[0], e[1] * rhs.e[1], e[2] * rhs.e[2]);
}

const vec3 vec3::operator/(const vec3& rhs) const {
    return vec3(e[0] / rhs.e[0], e[1] / rhs.e[1], e[2] / rhs.e[2]);
}

const vec3 vec3::operator-() const { return vec3(-e[0], -e[1], -e[2]); }

float* vec3::data() { return e; }

const float* vec3::data() const { return e; }

const float vec3::dot(const vec3& rhs) const {
    return e[0] * rhs.e[0] + e[1] * rhs.e[1] + e[2] * rhs.e[2];
}

float& vec3::x() { return e[0]; }
float& vec3::y() { return e[1]; }
float& vec3::z() { return e[2]; }
const float& vec3::x() const { return e[0]; }
const float& vec3::y() const { return e[1]; }
const float& vec3::z() const { return e[2]; }

float& vec3::r() { return e[0]; }
float& vec3::g() { return e[1]; }
float& vec3::b() { return e[2]; }
const float& vec3::r() const { return e[0]; }
const float& vec3::g() const { return e[1]; }
const float& vec3::b() const { return e[2]; }

/********************** vec2 **********************/
vec2::vec2() { e[0] = e[1] = 0; }

vec2::vec2(float k) { e[0] = e[1] = k; }

vec2::vec2(const float* data) {
    e[0] = data[0];
    e[1] = data[1];
}
vec2::vec2(float x, float y) {
    e[0] = x;
    e[1] = y;
}

float vec2::length_squared() const { return e[0] * e[0] + e[1] * e[1]; }

float vec2::length() const { return sqrt(length_squared()); }

const vec2 vec2::normalized() const { return *this / length(); }

const vec2 vec2::operator+(const vec2& rhs) const {
    return vec2(e[0] + rhs.e[0], e[1] + rhs.e[1]);
}

const vec2 vec2::operator-(const vec2& rhs) const { return *this + (-rhs); }

const vec2 vec2::operator*(const vec2& rhs) const {
    return vec2(e[0] * rhs.e[0], e[1] * rhs.e[1]);
}

const vec2 vec2::operator/(const vec2& rhs) const {
    return vec2(e[0] / rhs.e[0], e[1] / rhs.e[1]);
}

const vec2 vec2::operator-() const { return vec2(-e[0], -e[1]); }

float& vec2::x() { return e[0]; }
float& vec2::y() { return e[1]; }
const float& vec2::x() const { return e[0]; }
const float& vec2::y() const { return e[1]; }

float& vec2::u() { return e[0]; }
float& vec2::v() { return e[1]; }
const float& vec2::u() const { return e[0]; }
const float& vec2::v() const { return e[1]; }

float* vec2::data() { return e; }

const float* vec2::data() const { return e; }

/********************** mat3 **********************/
mat3::mat3() {
    for(int i = 0; i < 9; i++) e[i] = 0.0f;
    for(int i = 0; i < 3; i++) e[i * 3 + i] = 1.0f;
}

mat3::mat3(float k) {
    for(int i = 0; i < 9; i++) e[i] = 0.0f;
    for(int i = 0; i < 3; i++) e[i * 3 + i] = k;
}

mat3::mat3(const float* data) {
    for(int i = 0; i < 9; i++) {
        e[i] = data[i];
    }
}

mat3::mat3(float m00, float m01, float m02, float m10, float m11, float m12,
           float m20, float m21, float m22) {
    e[0] = m00;
    e[1] = m01;
    e[2] = m02;
    e[3] = m10;
    e[4] = m11;
    e[5] = m12;
    e[6] = m20;
    e[7] = m21;
    e[8] = m22;
}

mat3::mat3(vec3 v0, vec3 v1, vec3 v2) {
    e[0] = v0.x();
    e[1] = v1.x();
    e[2] = v2.x();
    e[3] = v0.y();
    e[4] = v1.y();
    e[5] = v2.y();
    e[6] = v0.z();
    e[7] = v1.z();
    e[8] = v2.z();
}

const mat3 mat3::transpose() const {
    mat3 res;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            res.e[i * 3 + j] = e[j * 3 + i];
        }
    }
    return res;
}

float* mat3::data() { return e; }

const float* mat3::data() const { return e; }

const mat3 mat3::inverse() const {
    mat3 res;
    auto det22 = [&](int A, int B, int C, int D) {
        return e[A] * e[D] - e[C] * e[B];
    };
    float det33v = e[0] * det22(4, 5, 7, 8) - e[1] * det22(3, 5, 6, 8) +
                   e[2] * det22(3, 4, 6, 7);
    float rd = 1.0f / det33v;
    res.e[0] = rd * det22(4, 5, 7, 8);
    res.e[3] = rd * -det22(3, 5, 6, 8);
    res.e[6] = rd * det22(3, 4, 6, 7);
    res.e[1] = rd * -det22(1, 2, 7, 8);
    res.e[4] = rd * det22(0, 2, 6, 8);
    res.e[7] = rd * -det22(0, 1, 6, 7);
    res.e[2] = rd * det22(1, 2, 4, 5);
    res.e[5] = rd * -det22(0, 2, 3, 5);
    res.e[8] = rd * det22(0, 1, 3, 4);
    return res;
}

const vec3 mat3::mul_vec3(const vec3& rhs) const {
    return vec3(e[0] * rhs.x() + e[1] * rhs.y() + e[2] * rhs.z(),
                e[3] * rhs.x() + e[4] * rhs.y() + e[5] * rhs.z(),
                e[6] * rhs.x() + e[7] * rhs.y() + e[8] * rhs.z());
}

const mat3 mat3::operator*(const mat3& rhs) const {
    mat3 res(0.0f);
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            for(int k = 0; k < 3; k++) {
                res.e[i * 3 + j] += e[i * 3 + k] * rhs.e[k * 3 + j];
            }
        }
    }
    return res;
}

const mat3 mat3::operator+(const mat3& rhs) const {
    mat3 res(0.0f);
    for(int i = 0; i < 9; i++) res.e[i] = e[i] + rhs.e[i];
    return res;
}

const mat3 mat3::dot(const mat3& rhs) const {
    mat3 res(0.0f);
    for(int i = 0; i < 9; i++) res.e[i] = e[i] * rhs.e[i];
    return res;
}

const mat3 operator*(const mat3& rhs, float k) { return rhs.dot(mat3(k)); }

const mat3 operator*(float k, const mat3& rhs) { return rhs.dot(mat3(k)); }

float& mat3::at(int x, int y) {
    assert(x < 3 && x >= 0 && y < 3 && y >= 0);
    return e[x + y * 3];
}

const float& mat3::at(int x, int y) const {
    assert(x < 3 && x >= 0 && y < 3 && y >= 0);
    return e[x + y * 3];
}

/********************** mat4 **********************/

mat4::mat4() {
    for(int i = 0; i < 16; i++) e[i] = 0.0f;
    for(int i = 0; i < 4; i++) e[i * 4 + i] = 1.0f;
}

mat4::mat4(float k) {
    for(int i = 0; i < 16; i++) e[i] = 0.0f;
    for(int i = 0; i < 4; i++) e[i * 4 + i] = k;
}

mat4::mat4(const float* data) {
    for(int i = 0; i < 16; i++) e[i] = data[i];
}

mat4::mat4(float m00, float m01, float m02, float m03, float m10, float m11,
           float m12, float m13, float m20, float m21, float m22, float m23,
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

const vec4 mat4::mul_vec4(const vec4& rhs) const {
    return vec4(
        e[0] * rhs.x() + e[1] * rhs.y() + e[2] * rhs.z() + e[3] * rhs.w(),
        e[4] * rhs.x() + e[5] * rhs.y() + e[6] * rhs.z() + e[7] * rhs.w(),
        e[8] * rhs.x() + e[9] * rhs.y() + e[10] * rhs.z() + e[11] * rhs.w(),
        e[12] * rhs.x() + e[13] * rhs.y() + e[14] * rhs.z() + e[15] * rhs.w());
}

const mat4 mat4::operator+(const mat4& rhs) const {
    mat4 res(0.0f);
    for(int i = 0; i < 16; i++) res.e[i] = e[i] + rhs.e[i];
    return res;
}

const mat4 mat4::operator*(const mat4& rhs) const {
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

const mat4 mat4::transpose() const {
    mat4 res;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            res.e[i * 4 + j] = e[j * 4 + i];
        }
    }
    return res;
}

const mat4 mat4::dot(const mat4& rhs) const {
    mat4 res(0.0f);
    for(int i = 0; i < 16; i++) res.e[i] = e[i] * rhs.e[i];
    return res;
}

const mat4 operator*(const mat4& rhs, float k) { return rhs.dot(mat4(k)); }

const mat4 operator*(float k, const mat4& rhs) { return rhs.dot(mat4(k)); }

float* mat4::data() { return e; }

const float* mat4::data() const { return e; }

float& mat4::at(int x, int y) {
    assert(x < 4 && x >= 0 && y < 4 && y >= 0);
    return e[x + y * 4];
}

const float& mat4::at(int x, int y) const {
    assert(x < 4 && x >= 0 && y < 4 && y >= 0);
    return e[x + y * 4];
}

/********************** Tool Functions **********************/
// clang-format off
const vec3 cross(const vec3& lhs, const vec3& rhs) {
    return vec3(lhs.y() * rhs.z() - rhs.y() * lhs.z(),
                lhs.z() * rhs.x() - rhs.z() * lhs.x(),
                lhs.x() * rhs.y() - rhs.x() * lhs.y());
}

const mat4 translate(vec3 tr) {
    return mat4(1.0f, 0.0f, 0.0f, tr.x(), 
                0.0f, 1.0f, 0.0f, tr.y(), 
                0.0f, 0.0f, 1.0f, tr.z(), 
                0.0f, 0.0f, 0.0f, 1.0f);
}

const mat4 scale(vec3 sc) {
    return mat4(sc.x(), 0.0f, 0.0f, 0.0f, 
                0.0f, sc.y(), 0.0f, 0.0f, 
                0.0f, 0.0f, sc.z(), 0.0f, 
                0.0f, 0.0f, 0.0f, 1.0f);
}

const mat4 ortho(float n, float f, float fov, float aspect) {
    float t = tan(radian(fov) / 2.0f) * f;
    float r = aspect * t;
    return mat4(1.0f / r, 0.0f, 0.0f, 0.0f, 
                0.0f, 1.0f / t, 0.0f, 0.0f, 0.0f,
                0.0f, 2.0f / (n - f), (n + f) / (n - f), 
                0.0f, 0.0f, 0.0f, 1.0f);
}

// fov: degree
const mat4 perspective(float n, float f, float fov, float aspect) {
    float t = tan(radian(fov) / 2.0f) * n;
    float r = aspect * t;
    return mat4(n / r, 0.0f, 0.0f, 0.0f, 
                0.0f, n / t, 0.0f, 0.0f, 
                0.0f, 0.0f, (n + f) / (n - f), 2.0f * n * f / (n - f), 
                0.0f, 0.0f, -1.0f, 0.0f);
}

const mat4 rotate(vec3 axis, float angle) {
    angle = radian(angle);
    mat3 res(1.0f);
    mat3 N(0.0f, -axis.z(), axis.y(), axis.z(), 0.0f, -axis.x(), -axis.y(),
           axis.x(), 0.0f);
    mat3 maxis(axis.x() * axis.x(), axis.x() * axis.y(), axis.x() * axis.z(),
               axis.y() * axis.x(), axis.y() * axis.y(), axis.y() * axis.z(),
               axis.z() * axis.x(), axis.z() * axis.y(), axis.z() * axis.z());

    res = res * mat3(cos(angle)) + maxis * mat3(1 - cos(angle)) +
          N * mat3(sin(angle));
    return extend_mat3(res);
}

const mat4 euler_YXZ_rotate(vec3 rotation) {
    return rotate(vec3(0.0f, 1.0f, 0.0f), rotation.y()) *
           rotate(vec3(1.0f, 0.0f, 0.0f), rotation.x()) *
           rotate(vec3(0.0f, 0.0f, 1.0f), rotation.z());
}

const mat4 lookat(vec3 eye, vec3 at, vec3 up) {
    vec3 z = (eye - at).normalized();
    vec3 x = cross(up, z).normalized();
    vec3 y = cross(z, x).normalized();
    return mat4(x.x(), x.y(), x.z(), -eye.dot(x), 
                y.x(), y.y(), y.z(), -eye.dot(y), 
                z.x(), z.y(), z.z(), -eye.dot(z), 
                0.0f, 0.0f, 0.0f, 1.0f);
}

const mat4 viewport(int width, int height) {
    return mat4(width / 2.0f, 0.0f, 0.0f, width / 2.0f, 
                0.0f, height / 2.0f, 0.0f, height / 2.0f, 
                0.0f, 0.0f, 1.0f, 0.0f, 
                0.0f, 0.0f, 0.0f, 1.0f);
}

const mat3 clip_mat4(const mat4& m) {
    const float* data = m.data();
    return mat3(data[0], data[1], data[2], 
                data[4], data[5], data[6], 
                data[8], data[9], data[10]);
}

const mat4 extend_mat3(const mat3& m) {
    return mat4(m.at(0, 0), m.at(1, 0), m.at(2, 0), 0, 
                m.at(0, 1), m.at(1, 1), m.at(2, 1), 0, 
                m.at(0, 2), m.at(1, 2), m.at(2, 2), 0, 
                0, 0, 0, 1);
}

// clang-format on