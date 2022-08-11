#include "mathbase.h"
#include "Shader.h"

class Camera {
public:
    Camera();
    void set_translation(vec3 translation);
    void set_rotation(vec3 rotation);
    mat4 get_view_matrix();
    virtual mat4 get_camera_matrix() = 0;
    virtual void transfer(Shader* shader) = 0;
protected:
    vec3 translation;
    vec3 rotation;
};


class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera();
    void transfer(Shader* shader);
    mat4 get_camera_matrix();
    void set(const float* n, const float* f, const float* fov, const float* ratio);
    void get(float* n, float* f, float* fov, float* ratio);
private:
    float n, f;
    float fov, ratio;
};