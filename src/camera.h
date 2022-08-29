#ifndef RASTERIZER_CAMERA_H_
#define RASTERIZER_CAMERA_H_

#include "mathbase.h"
#include "shader.h"

class Camera {
public:
    Camera();
    void set_translation(vec3 translation);
    void set_rotation(vec3 rotation);
    mat4 get_view_matrix();
    virtual mat4 get_camera_matrix() = 0;
    virtual void transfer(Shader* shader) = 0;
    virtual ~Camera();
protected:
    vec3 translation;
    vec3 rotation;
};


class PerspectiveCamera : public Camera {
public:
    /**
     * @brief 透视相机
     * 
     */
    PerspectiveCamera();
    
    /**
     * @brief 传递相机参数到shader中
     * 
     * @param shader 相机矩阵传入到location=1的uniform变量中；相机位置传入到location=2的uniform变量中
     */
    void transfer(Shader* shader);

    mat4 get_camera_matrix();
    /**
     * @brief 设置相机参数。传入NULL代表无需修改 
     * 
     * @param n 进平面距离（正值）
     * @param f 远平面距离（正值）
     * @param fov 视角，角度制
     * @param ratio 宽高比
     */
    void set(const float* n, const float* f, const float* fov, const float* ratio);

    /**
     * @brief 获得相机参数。传入NULL代表无需获得
     * 
     * @param n 进平面距离
     * @param f 远平面距离
     * @param fov 视角，角度制
     * @param ratio 宽高比
     */
    void get(float* n, float* f, float* fov, float* ratio);

private:
    float n, f;
    float fov, ratio;
};

#endif // RASTERIZER_CAMERA_H_