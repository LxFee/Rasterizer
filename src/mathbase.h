#ifndef RASTERIZER_MATHBASE_H_
#define RASTERIZER_MATHBASE_H_

#define PI 3.141592653589793238
#define INV_PI 0.318309886183790691

class vec3 {
public:
    vec3(float k = 0.0f);
    explicit vec3(const float* ep);
    explicit vec3(float x, float y, float z);
    float length_squared() const ;
    float length() const ;
    const vec3 normalized() const ;
    
    const vec3 operator + (const vec3& rhs) const;
    const vec3 operator - () const ;
    const vec3 operator - (const vec3& rhs) const;
    const vec3 operator / (float k) const ; 
    
    float* data();
    const float* data() const ;

    float x() const; 
    float y() const; 
    float z() const;
    
    float r() const; 
    float g() const; 
    float b() const;

private:
    float e[3];
};

const vec3 operator*(const vec3& lhs, const vec3& rhs);

const vec3 cross(const vec3 &lhs, const vec3 &rhs);

float dot(const vec3 &lhs, const vec3 &rhs);


class vec4 {
public:
    vec4(float k = 0.0f);
    explicit vec4(const float* ep);
    explicit vec4(float x, float y, float z, float w);
    explicit vec4(const vec3& v, float w);
    float length_squared() const ;
    float length() const ;
    const vec4 normalized() const ;
    
    const vec4 operator + (const vec4& rhs) const;
    const vec4 operator - () const ;
    const vec4 operator - (const vec4& rhs) const;
    const vec4 operator / (float k) const ; 
    
    float* data();
    const float* data() const ;

    float x() const; 
    float y() const; 
    float z() const;
    float w() const;
    
    float r() const; 
    float g() const; 
    float b() const;
    float a() const;
private:
    float e[4];
};

const vec4 operator*(const vec4& lhs, const vec4& rhs);


struct vec2 {
public:
    vec2(float k = 0.0f);
    explicit vec2(const float* ep);
    explicit vec2(float x, float y);
    float length_squared() const ;
    float length() const ;
    const vec2 normalized() const ;
    
    const vec2 operator + (const vec2& rhs) const;
    const vec2 operator - () const ;
    const vec2 operator - (const vec2& rhs) const;
    const vec2 operator / (float k) const ; 
    
    float* data();
    const float* data() const ;

    float x() const; 
    float y() const;  

    float u() const; 
    float v() const;
private:
    float e[2];
};


const vec2 operator*(const vec2 &lhs, const vec2& rhs);

float det(const vec2 &lhs, const vec2 &rhs);


class mat3 {
public:
    explicit mat3(float k = 0.0f);
    explicit mat3(const float* ep);
    explicit mat3(  float m00, float m01, float m02,
                    float m10, float m11, float m12,
                    float m20, float m21, float m22);
    explicit mat3(vec3 v0, vec3 v1, vec3 v2);
    mat3 T() const;
    
    float* data();
    const float* data() const;

    float det() const;

    const mat3 inv() const ;
    const mat3 operator * (const mat3& rhs) const ;
    const vec3 operator * (const vec3& rhs) const ;
    const mat3 operator + (const mat3& rhs) const;

    const mat3 dot(const mat3& rhs) const;
private:
    float e[9];
};

const mat3 operator * (const mat3& rhs, float k);

const mat3 operator * (float k, const mat3& rhs);



class mat4 {
public:
    explicit mat4(float k = 0.0f);
    explicit mat4(const float* ep);
    explicit mat4(const mat3& m);
    explicit mat4(  float m00, float m01, float m02, float m03,
                    float m10, float m11, float m12, float m13,
                    float m20, float m21, float m22, float m23,
                    float m30, float m31, float m32, float m33);

    const mat4 T() const;

    float* data();
    const float* data() const;

    const vec4 operator * (const vec4& rhs) const ;
    const mat4 operator * (const mat4& rhs) const ;
    const mat4 operator + (const mat4& rhs) const;
    const mat4 dot(const mat4& rhs) const;

private:
    float e[16];
};

const mat4 operator * (const mat4& rhs, float k);

const mat4 operator * (float k, const mat4& rhs);

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

const mat4 translate(vec3 tr);

const mat4 scale(vec3 sc);

const mat4 euler_YXZ_rotate(vec3 rotation);

const mat4 rotate(vec3 axis, float angle);

const mat4 ortho(float l, float r, float b, float t, float n, float f);

const mat4 perspective(float n, float f, float fov, float aspect);

const mat4 lookat(vec3 eye, vec3 at, vec3 up);

const mat3 clip_translate(const mat4& m);


inline float radian(float d) {
    return d * PI / 180.0f;
}

inline float degree(float r) {
    return r * 180.0f * INV_PI;
}

#endif // RASTERIZER_MATHBASE_H_