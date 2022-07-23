#pragma once


struct vec4 {
    vec4();
    vec4(float k);
    vec4(float x, float y, float z, float w = 1.0f);
    float length_squared() const ;
    float length() const ;
    vec4 normalized() const ;
    
    vec4 operator + (const vec4& rhs) const;
    vec4 operator - () const ;
    vec4 operator - (const vec4& rhs) const;
    vec4 operator * (const vec4& rhs) const;
    vec4 operator / (float k) const ; 
    
    float x() const; 
    float y() const; 
    float z() const;
    float w() const;
    
    float r() const; 
    float g() const; 
    float b() const;
    float a() const;

    float e[4];
};

struct vec3 {
    vec3();
    vec3(float k);
    vec3(float x, float y, float z);
    float length_squared() const ;
    float length() const ;
    vec3 normalized() const ;
    
    vec3 operator + (const vec3& rhs) const;
    vec3 operator - () const ;
    vec3 operator - (const vec3& rhs) const;
    vec3 operator * (const vec3& rhs) const;
    vec3 operator / (float k) const ; 
    
    float x() const; 
    float y() const; 
    float z() const;
    
    float r() const; 
    float g() const; 
    float b() const;

    float e[3];
};

struct vec2 {
    vec2();
    vec2(float k);
    vec2(float x, float y);
    float length_squared() const ;
    float length() const ;
    vec2 normalized() const ;
    
    vec2 operator + (const vec2& rhs) const;
    vec2 operator - () const ;
    vec2 operator - (const vec2& rhs) const;
    vec2 operator * (const vec2& rhs) const;
    vec2 operator / (float k) const ; 
    
    float x() const; 
    float y() const; 

    float e[2];
};

struct mat4 {
    mat4(float k = 0.0f);
    mat4(   float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33);
    mat4(vec4 v0, vec4 v1, vec4 v2, vec4 v3);

    mat4 T() const;

    vec4 operator * (const vec4& rhs) const ;

    mat4 operator * (const mat4& rhs) const ;

    float e[16];
};

float clamp(float x, float mi, float mx);

int packRGBA8888(vec4 col);