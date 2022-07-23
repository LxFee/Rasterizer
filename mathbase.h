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

float clamp(float x, float mi, float mx);

int packRGBA8888(vec4 col);