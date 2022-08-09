#include "camera.h"

Camera::Camera() : translation(0.0f), rotation(0.0f) {}

void Camera::set_translation(vec3 translation) {
    this->translation = translation;
}


void Camera::set_rotation(vec3 rotation) {
    this->rotation = rotation;
}


mat4 Camera::get_view_matrix() {
    return euler_YXZ_rotate(rotation).T() * translate(-translation);
}

PerspectiveCamera::PerspectiveCamera() : n(1.0f), f(100.0f), fov(90.0f), ratio(16.0f / 9.0f) {}

mat4 PerspectiveCamera::get_camera_matrix() {
    return perspective(n, f, fov, ratio) * get_view_matrix();
}

void PerspectiveCamera::transfer(Shader* shader) {
    shader->uniform(get_camera_matrix(), 1);
    shader->uniform(translation, 2);
}

void PerspectiveCamera::set(const float* n, const float* f, const float* fov, const float* ratio) {
    if(n)     this->n     = *n;
    if(f)     this->f     = *f;
    if(fov)   this->fov   = *fov;
    if(ratio) this->ratio = *ratio;
}

void PerspectiveCamera::get(float* n, float* f, float* fov, float* ratio) {
    if(n)     *n     = this->n;
    if(f)     *f     = this->f;
    if(fov)   *fov   = this->fov;
    if(ratio) *ratio = this->ratio;
}