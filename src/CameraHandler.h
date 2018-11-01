#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include <SDL2/SDL.h>
#include "use_eigen.h"

class CameraHandler {
    using Quat = Eigen::Quaterniond;
    using Vec3 = Eigen::Vector3d;
    using Transform = Eigen::Affine3d;
    using AngleAxisd = Eigen::AngleAxisd;

public:
    CameraHandler();

    /*
     * Handle an event
     * true if the event is handled
     */
    bool handleEvent(SDL_Event &event);

    /**
     * Apply internal state to view matrix
     */
    void applyViewMatrix();

private:
    const double DEG_PER_PIXEL = 0.2;
    const double STEP_MOVE = 0.003;
    const double STEP_ROT = DEG_PER_PIXEL * M_PI / 180;
    const double STEP_PAN = 0.003;
    const int DISTANCE_BASE = 5;

    enum State {
        STATE_MOUSE_LEFT,
        STATE_MOUSE_MIDDLE,
        STATE_MOUSE_RIGHT,
        STATE_KEY_ALT,
        STATE_KEY_SHIFT,
        STATE_KEY_CTRL,
        STATE_KEY_SPACE,
        STATE_KEY_W,
        STATE_KEY_A,
        STATE_KEY_D,
        STATE_KEY_S,
        STATE_KEY_Q,
        STATE_KEY_E,
        STATE_MAX
    };

    enum ActionRequest {
        ACTION_NOTHING,
        ACTION_FOCUS,
    };

    // current state
    ActionRequest action = ACTION_NOTHING;
    bool state[STATE_MAX]{};
    int deltaX, deltaY;
    uint32_t lastTicks = 0;
    Transform transform;

    // zooming
    double zoom = 0;
    double distance = DISTANCE_BASE;

    void tick();
    void setRelativeMouseMode() const;
};

#endif //CAMERAHANDLER_H
