#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include <SDL2/SDL.h>
#include "utils/use-eigen.h"

class CameraHandler {
    using Quat = Eigen::Quaterniond;
    using Vec3 = Eigen::Vector3f;
    using Transform = Eigen::Affine3f;
    using AngleAxisd = Eigen::AngleAxisf;

public:
    CameraHandler();

    /*
     * Handle an event
     * true if the event is handled
     */
    bool handleEvent(SDL_Event &event);

    /**
     * Called from Application to update internal state
     */
    void tick();

    /**
     * Apply internal state to view matrix
     */
    const float *getViewMatrix() const;

private:
    const double DEG_PER_PIXEL = 0.2;
    const double STEP_MOVE = 0.003;
    const double STEP_ROT = DEG_PER_PIXEL * M_PI / 180;
    const double STEP_PAN = 0.003;
    const int DISTANCE_BASE = 3;

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

    void setRelativeMouseMode() const;
    inline bool shouldPreventWasd() const;
};

#endif //CAMERAHANDLER_H
