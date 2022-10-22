#include "core/pinnedcamera.h"
#include <cstring>
#include "utils/GStorage.h"


/** pinned_camera_t **/

namespace {

const vec3 UP(0.0f, 1.0f, 0.0f);

typedef struct {
    vec2 orbit;
    vec2 pan;
    float dolly;
} motion_t;


vec3 calculate_pan(vec3 from_camera, float fov, motion_t motion) {
    vec3 forward = from_camera.normalized();
    vec3 left = cross(UP, forward).normalized();
    vec3 up = cross(forward, left).normalized();

    float distance = from_camera.length();
    float factor = distance * (float)tan(radian(fov / 2.0f)) * 2.0f;
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

}  // namespace

pinned_camera_t::pinned_camera_t(float _aspect, projection_mode_t _mode) : camera_t(_aspect, _mode),
      position(0.0f, 0.0f, 5.0f),
      target(0.0f, 0.0f, 0.0f) {}

void pinned_camera_t::set_transform(vec3 _position, vec3 _target) {
    position = _position;
    target = _target;
}

void pinned_camera_t::update_transform() {
    pinned_camera_record_t& record = get_record();

    motion_t motion;
    motion.orbit = record.orbitDelta;
    motion.pan = record.panDelta;
    motion.dolly = record.dollyDelta;

    vec3 from_target = position - target;
    vec3 from_camera = target - position;
    
    vec3 pan = calculate_pan(from_camera, get_zoom(), motion);
    vec3 offset = calculate_offset(from_target, motion);
    target = target + pan;
    position = target + offset;

    clear_record();
}

vec3 pinned_camera_t::get_position() const { return position; }
vec3 pinned_camera_t::get_target() const { return target; }

const mat4 pinned_camera_t::get_view_matrix() const {
    return lookat(position, target, UP);
}

pinned_camera_t::pinned_camera_record_t& pinned_camera_t::get_record() {
    static pinned_camera_record_t record;
    return record;
}

void pinned_camera_t::clear_record() {
    pinned_camera_record_t& record = get_record();
    record.orbitDelta = vec2(0, 0);
    record.panDelta = vec2(0, 0);
    record.dollyDelta = 0;
    record.singleClick = 0;
    record.doubleClick = 0;
}

void pinned_camera_t::register_input() {
    pinned_camera_record_t& record = get_record();

    /* callbacks */
    EventManager::registerEvent(Events::MOUSE_SCROLL, [&](window_t* window, float offset) {
        record.dollyDelta += offset;
    });

    EventManager::registerEvent(Events::MOUSE_BUTTON_LEFT | Events::MOUSE_BUTTON_PRESS, [&](window_t* window) {
        float currTime = platform_get_time();
        float cursorX, cursorY;
        input_query_cursor(NULL, &cursorX, &cursorY);
        vec2 cursorPos(cursorX, cursorY);
        record.isOrbiting = 1;
        record.orbitPos = cursorPos;
        record.pressTime = currTime;
        record.pressPos = cursorPos;
    });

    EventManager::registerEvent(Events::MOUSE_BUTTON_LEFT | Events::MOUSE_BUTTON_RELEASE, [&](window_t* window) {
        static const float CLICK_DELAY = 0.25f;
        int width, height;
        window_query_size(window, &width, &height);
        float currTime = platform_get_time();
        float cursorX, cursorY;
        input_query_cursor(NULL, &cursorX, &cursorY);
        vec2 cursorPos(cursorX, cursorY);
        float prevTime = record.releaseTime;
        vec2 posDelta = (cursorPos - record.orbitPos) / (float)height;
        record.isOrbiting = 0;
        record.orbitDelta = record.orbitDelta + posDelta;
        if(prevTime && currTime - prevTime < CLICK_DELAY) {
            record.doubleClick = 1;
            record.releaseTime = 0;
        } else {
            record.releaseTime = currTime;
            record.releasePos = cursorPos;
        }
    });

    EventManager::registerEvent(Events::MOUSE_BUTTON_RIGHT | Events::MOUSE_BUTTON_PRESS, [&](window_t* window) {
        float cursorX, cursorY;
        input_query_cursor(NULL, &cursorX, &cursorY);
        vec2 cursorPos(cursorX, cursorY);
        record.isPanning = 1;
        record.panPos = cursorPos;
    });

    EventManager::registerEvent(Events::MOUSE_BUTTON_RIGHT | Events::MOUSE_BUTTON_RELEASE, [&](window_t* window) {
        int width, height;
        window_query_size(window, &width, &height);
        float cursorX, cursorY;
        input_query_cursor(NULL, &cursorX, &cursorY);
        vec2 cursorPos(cursorX, cursorY);
        vec2 posDelta = (cursorPos - record.panPos) / (float)height;
        record.isPanning = 0;
        record.panDelta = record.panDelta + posDelta;
    });
}