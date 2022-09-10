#include <cassert>
#include <cmath>
#include "camera.h"
#include "maths.h"

/** camera_t **/
camera_t::camera_t(float _aspect, projection_mode_t _mode) 
    : n(1.0f), f(50.0f), zoom(60.0f), aspect(_aspect), mode(_mode) {}

camera_t::~camera_t() {}

const mat4 camera_t::get_projection_matrix() const {
    switch(mode) {
        case PROJECTION_MODE_ORTHO: 
            return ortho(n, f, zoom, aspect);
        case PROJECTION_MODE_PERSPECTIVE: 
            return perspective(n, f, zoom, aspect);
        default: 
            assert(0);
            return 0;
    }
}

void camera_t::set_near(float _n) {n = _n;}
void camera_t::set_far(float _f) {f = _f;}
void camera_t::set_zoom(float _zoom) {zoom = _zoom;}
void camera_t::set_aspect(float _aspect) {aspect = _aspect;}
void camera_t::set_mode(projection_mode_t _mode) {mode = _mode;}

float camera_t::get_near() const {return n;}
float camera_t::get_far() const {return f;}
float camera_t::get_zoom() const {return zoom;}
float camera_t::get_aspect() const {return aspect;}
projection_mode_t camera_t::get_mode() const {return mode;}


/** pinned_camera_t **/

namespace {
    namespace pinned_camera {
    
    const vec3 UP(0.0f, 1.0f, 0.0f);

    vec3 calculate_pan(vec3 from_camera, float fov, motion_t motion) {
        vec3 forward = from_camera.normalized();
        vec3 left =  cross(UP, forward);
        vec3 up = cross(forward, left);

        float distance = from_camera.length();
        float factor = distance * (float)tan(fov / 2.0f) * 2.0f;
        vec3 delta_x = left * (motion.pan.x() * factor);
        vec3 delta_y = up * (motion.pan.y() * factor);
        return delta_x + delta_y;
    }

    vec3 calculate_offset(vec3 from_target, motion_t motion) {
        float radius = from_target.length();
        float theta = (float)atan2(from_target.x(), from_target.z());
        float phi = (float)acos(from_target.y() / radius);
        float factor = PI * 2;
        vec3 offset;

        radius *= (float)pow(0.95, motion.dolly);
        theta -= motion.orbit.x() * factor;
        phi -= motion.orbit.y() * factor;
        phi = clamp(phi, EPSILON, PI - EPSILON);

        offset.x() = radius * (float)sin(phi) * (float)sin(theta);
        offset.y() = radius * (float)cos(phi);
        offset.z() = radius * (float)sin(phi) * (float)cos(theta);

        return offset;
    }

    }

};

pinned_camera_t::pinned_camera_t(float _aspect, projection_mode_t _mode) 
    : camera_t(_aspect, _mode), position(0.0f, 0.0f, 0.0f), target(0.0f, 0.0f, -1.0f) {}

void pinned_camera_t::set_transform(vec3 _position, vec3 _target) {
    position = _position;
    target = _target;
}

void pinned_camera_t::update_transform(motion_t motion) {
    vec3 from_target = position - target;
    vec3 from_camera = target - position;

    vec3 pan = pinned_camera::calculate_pan(from_camera, get_zoom(), motion);
    vec3 offset = pinned_camera::calculate_offset(from_target, motion);
    target = target + pan;
    position = target + offset;
}

const mat4 pinned_camera_t::get_view_matrix() const {
    return lookat(position, target, pinned_camera::UP);
}