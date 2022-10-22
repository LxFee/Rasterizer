#ifndef RASTERIZER_PINNEDCAMERA_H_
#define RASTERIZER_PINNEDCAMERA_H_

#include "camera.h"
#include "core/platform.h"


class pinned_camera_t : public camera_t {
   public:
    pinned_camera_t(float _aspect, projection_mode_t _mode);
    void set_transform(vec3 _position, vec3 _target);
    void update_transform(window_t* window);
    vec3 get_position() const;
    vec3 get_target() const;
    const mat4 get_view_matrix() const override;
    static void register_input();
    static void clear_record();

   private:
    struct pinned_camera_record_t {
        pinned_camera_record_t() :
            isOrbiting(0),
            orbitPos(),
            orbitDelta(),
            isPanning(0),
            panPos(),
            panDelta(),
            dollyDelta(0),
            releaseTime(0),
            pressPos(),
            releasePos(),
            singleClick(0),
            doubleClick(0),
            clickPos() {}
        
        /* orbit */
        int isOrbiting;
        vec2 orbitPos;
        vec2 orbitDelta;
        /* pan */
        int isPanning;
        vec2 panPos;
        vec2 panDelta;
        /* zoom */
        float dollyDelta;
        /* click */
        float pressTime;
        float releaseTime;
        vec2 pressPos;
        vec2 releasePos;
        int singleClick;
        int doubleClick;
        vec2 clickPos;
    } ;

    static pinned_camera_record_t& get_record();
    vec3 position;
    vec3 target;
};

#endif // RASTERIZER_PINNEDCAMERA_H_