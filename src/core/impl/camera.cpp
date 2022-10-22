#include "core/camera.h"

#include <cassert>
#include <cmath>

#include "core/maths.h"

/** camera_t **/
camera_t::camera_t(float _aspect, projection_mode_t _mode)
    : n(0.1f), f(200.0f), zoom(60.0f), aspect(_aspect), mode(_mode) {}

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

void camera_t::set_near(float _n) { n = _n; }
void camera_t::set_far(float _f) { f = _f; }
void camera_t::set_zoom(float _zoom) { zoom = _zoom; }
void camera_t::set_aspect(float _aspect) { aspect = _aspect; }
void camera_t::set_mode(projection_mode_t _mode) { mode = _mode; }

float camera_t::get_near() const { return n; }
float camera_t::get_far() const { return f; }
float camera_t::get_zoom() const { return zoom; }
float camera_t::get_aspect() const { return aspect; }
projection_mode_t camera_t::get_mode() const { return mode; }