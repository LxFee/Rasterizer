#ifndef RASTERIZER_MATHBASE_H_
#define RASTERIZER_MATHBASE_H_

#define PI 3.141592653589793238
#define INV_PI 0.318309886183790691

struct vec3 {
    vec3();
    vec3(float k);
    vec3(const float* ep);
    vec3(float x, float y, float z);
    float length_squared() const ;
    float length() const ;
    const vec3 normalized() const ;
    
    const vec3 operator + (const vec3& rhs) const;
    const vec3 operator - () const ;
    const vec3 operator - (const vec3& rhs) const;
    const vec3 operator * (const vec3& rhs) const;
    const vec3 operator / (float k) const ; 
    
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

    float e[3];
};

const vec3 cross(const vec3 &lhs, const vec3 &rhs);

float dot(const vec3 &lhs, const vec3 &rhs);

const vec3 operator*(float k, const vec3& rhs);

const vec3 operator*(const vec3& lhs, float k);


struct vec4 {
    vec4();
    vec4(float k);
    vec4(const float* ep);
    vec4(float x, float y, float z, float w = 1.0f);
    vec4(const vec3& v, float w = 1.0f);
    float length_squared() const ;
    float length() const ;
    const vec4 normalized() const ;
    
    const vec4 operator + (const vec4& rhs) const;
    const vec4 operator - () const ;
    const vec4 operator - (const vec4& rhs) const;
    const vec4 operator * (const vec4& rhs) const;
    const vec4 operator / (float k) const ; 
    
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

    float e[4];
};

const vec4 operator*(float k, const vec4& rhs);

const vec4 operator*(const vec4& lhs, float k);


struct vec2 {
    vec2();
    vec2(float k);
    vec2(const float* ep);
    vec2(float x, float y);
    float length_squared() const ;
    float length() const ;
    const vec2 normalized() const ;
    
    const vec2 operator + (const vec2& rhs) const;
    const vec2 operator - () const ;
    const vec2 operator - (const vec2& rhs) const;
    const vec2 operator * (const vec2& rhs) const;
    const vec2 operator / (float k) const ; 
    
    const float& x() const; 
    const float& y() const; 
    float& x(); 
    float& y(); 

    const float& u() const; 
    const float& v() const; 
    float& u(); 
    float& v(); 

    float e[2];
};


const vec2 operator*(float k, const vec2& rhs);

const vec2 operator*(const vec2& lhs, float k);

float det(const vec2 &lhs, const vec2 &rhs);


struct mat3 {
    mat3(float k = 0.0f);
    mat3(const float* ep);
    mat3(   float m00, float m01, float m02,
            float m10, float m11, float m12,
            float m20, float m21, float m22);
    mat3(vec3 v0, vec3 v1, vec3 v2);
    mat3 T() const;
    
    float det() const;

    const mat3 inv() const ;

    const mat3 operator * (float k) const;
    const vec3 operator * (const vec3& rhs) const ;
    const mat3 operator * (const mat3& rhs) const ;
    const mat3 operator + (const mat3& rhs) const;

    float e[9];
};

struct mat4 {
    mat4(float k = 0.0f);
    mat4(const float* ep);
    mat4(const mat3& m);
    mat4(   float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33);

    const mat4 T() const;

    const mat4 operator * (float k) const;
    const vec4 operator * (const vec4& rhs) const ;
    const mat4 operator * (const mat4& rhs) const ;
    const mat4 operator + (const mat4& rhs) const;

    float e[16];
};

float clamp(float x, float mi, float mx);

inline int packRGBA8888(vec4 col) {
    int r = static_cast<int>(255 * clamp(col.r(), 0.0, 1.0));
    int g = static_cast<int>(255 * clamp(col.g(), 0.0, 1.0));
    int b = static_cast<int>(255 * clamp(col.b(), 0.0, 1.0));
    int a = static_cast<int>(255 * clamp(col.a(), 0.0, 1.0));
    return a + (r << 8) + (g << 16) + (b << 24);
}

inline vec4 unpackRGBA8888(unsigned char *col) {
    return vec4(col[0] / 255.0f, col[1] / 255.0f, col[2] / 255.0f, col[3] / 255.0f);
}

inline vec4 unpackRGBA888(unsigned char *col) {
    return vec4(col[0] / 255.0f, col[1] / 255.0f, col[2] / 255.0f, 1.0f);
}

mat4 translate(vec3 tr);

mat4 scale(vec3 sc);

mat4 euler_YXZ_rotate(vec3 rotation);

mat4 rotate(vec3 axis, float angle);

mat4 ortho(float l, float r, float b, float t, float n, float f);

mat4 perspective(float n, float f, float fov, float aspect);

mat4 lookat(vec3 eye, vec3 at, vec3 up);

mat3 clip_translate(const mat4& m);


inline float radian(float d) {
    return d * PI / 180.0f;
}

inline float degree(float r) {
    return r * 180.0f * INV_PI;
}

#endif // RASTERIZER_MATHBASE_H_