#ifndef RASTERIZER_MATHS_H_
#define RASTERIZER_MATHS_H_

#include <cmath>

#include "marco.h"

class vec2 {
   public:
    /** constructor **/
    vec2();
    vec2(float k);
    vec2(float x, float y);
    explicit vec2(const float* data);

    /** operation **/
    const vec2 operator+(const vec2& rhs) const;
    const vec2 operator-(const vec2& rhs) const;
    const vec2 operator*(const vec2& rhs) const;
    const vec2 operator/(const vec2& rhs) const;
    const vec2 operator-() const;

    /** access to data **/
    float* data();
    const float* data() const;

    /** getter **/
    float length_squared() const;
    float length() const;
    const vec2 normalized() const;

    const float& x() const;
    const float& y() const;
    float& x();
    float& y();

    const float& u() const;
    const float& v() const;
    float& u();
    float& v();

   private:
    float e[2];
};

class vec3 {
   public:
    /** constructor **/
    vec3();
    vec3(float k);
    vec3(float x, float y, float z);
    explicit vec3(const float* data);

    /** operation **/
    const vec3 operator+(const vec3& rhs) const;
    const vec3 operator-(const vec3& rhs) const;
    const vec3 operator*(const vec3& rhs) const;
    const vec3 operator/(const vec3& rhs) const;
    const vec3 operator-() const;
    const float dot(const vec3& rhs) const;

    /** access to data **/
    float* data();
    const float* data() const;

    /** getter **/
    float length_squared() const;
    float length() const;
    const vec3 normalized() const;

    const float& x() const;
    const float& y() const;
    const float& z() const;
    float& x();
    float& y();
    float& z();

    const float& r() const;
    const float& g() const;
    const float& b() const;
    float& r();
    float& g();
    float& b();

   private:
    float e[3];
};

class vec4 {
   public:
    /** constructor **/
    vec4();
    vec4(float k);
    vec4(float x, float y, float z, float w);
    vec4(vec3 v, float w);
    explicit vec4(const float* data);

    /** operation **/
    const vec4 operator+(const vec4& rhs) const;
    const vec4 operator-(const vec4& rhs) const;
    const vec4 operator*(const vec4& rhs) const;
    const vec4 operator/(const vec4& rhs) const;
    const vec4 operator-() const;
    const float dot(const vec4& rhs) const;

    /** access to data **/
    float* data();
    const float* data() const;

    /** getter **/
    float length_squared() const;
    float length() const;
    const vec4 normalized() const;

    const float& x() const;
    const float& y() const;
    const float& z() const;
    const float& w() const;
    float& x();
    float& y();
    float& z();
    float& w();

    const float& r() const;
    const float& g() const;
    const float& b() const;
    const float& a() const;
    float& r();
    float& g();
    float& b();
    float& a();

   private:
    float e[4];
};

class mat3 {
   public:
    /** constructor **/
    mat3();
    mat3(float k);
    mat3(float m00, float m01, float m02, float m10, float m11, float m12,
         float m20, float m21, float m22);
    mat3(vec3 v0, vec3 v1, vec3 v2);
    explicit mat3(const float* data);

    /** operation **/
    const mat3 operator*(const mat3& rhs) const;
    const mat3 operator+(const mat3& rhs) const;
    const mat3 transpose() const;
    const mat3 inverse() const;
    const mat3 dot(const mat3& rhs) const;
    const vec3 mul_vec3(const vec3& rhs) const;

    /** access to data **/
    float* data();
    const float* data() const;

    /** getter **/
    const float& at(int x, int y) const;
    float& at(int x, int y);

   private:
    float e[9];
};

class mat4 {
   public:
    /** constructor **/
    mat4();
    mat4(float k);
    mat4(float m00, float m01, float m02, float m03, float m10, float m11,
         float m12, float m13, float m20, float m21, float m22, float m23,
         float m30, float m31, float m32, float m33);
    explicit mat4(const float* data);

    /** operation **/
    const mat4 operator*(const mat4& rhs) const;
    const mat4 operator+(const mat4& rhs) const;
    const mat4 transpose() const;
    const mat4 inverse() const;
    const mat4 dot(const mat4& rhs) const;
    const vec4 mul_vec4(const vec4& rhs) const;

    /** access to data **/
    float* data();
    const float* data() const;

    /** getter **/
    const float& at(int x, int y) const;
    float& at(int x, int y);

   private:
    float e[16];
};

/** utils **/

inline float clamp(float x, float mi, float mx) {
    if(x < mi) return mi;
    if(x > mx) return mx;
    return x;
}

inline uint rgba2rgbapack(const vec4& col) {
    uint r = static_cast<uint>(255 * clamp(col.r(), 0.0, 1.0));
    uint g = static_cast<uint>(255 * clamp(col.g(), 0.0, 1.0));
    uint b = static_cast<uint>(255 * clamp(col.b(), 0.0, 1.0));
    uint a = static_cast<uint>(255 * clamp(col.a(), 0.0, 1.0));
    return (r << 24) | (g << 16) | (b << 8) | a;
}

inline int sgn(int x) {
    if(x == 0) return 0;
    return x < 0 ? -1 : 1;
}

inline const vec4 rgbapack2rgba(const uchar* color) {
    return vec4(color[0] / 255.0f, color[1] / 255.0f, color[2] / 255.0f, color[3] / 255.0f);
}

inline const vec4 rgbpack2rgba(const uchar* color) {
    return vec4(color[0] / 255.0f, color[1] / 255.0f, color[2] / 255.0f, 1.0f);
}

inline float float_srgb2linear(float value) { 
    return (float)pow(value, 2.2); 
}

inline float float_linear2srgb(float value) {
    return (float)pow(value, 1 / 2.2);
}

inline vec3 vec3_linear2srgb(vec3 value) {
    return vec3(float_linear2srgb(value.x()),
                float_linear2srgb(value.y()),
                float_linear2srgb(value.z()));
}

inline float radian(float d) { return d / 180.0f * PI; }

inline float degree(float r) { return r / PI * 180.0f; }

const mat4 translate(vec3 _translate);
const mat4 scale(vec3 _scale);
const mat4 euler_YXZ_rotate(vec3 rotation);
const mat4 rotate(vec3 axis, float angle);

const mat4 ortho(float n, float f, float zoom, float aspect);
const mat4 perspective(float n, float f, float zoom, float aspect);
const mat4 lookat(vec3 eye, vec3 at, vec3 up);
const mat4 viewport(int width, int height);

const vec3 cross(const vec3& lhs, const vec3& rhs);

const mat3 clip_mat4(const mat4& m);
const mat4 extend_mat3(const mat3& m);

#endif  // RASTERIZER_MATHS_H_