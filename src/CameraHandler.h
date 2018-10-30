#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include <SDL2/SDL.h>

class CameraHandler {
public:
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
    const int DISTANCE_BASE = 5;
    const double DEG_PER_PIXEL = 0.35;

    // rotation
    double rotationY = 45;
    double rotationX = 45;

    // zooming
    double zoom = 0;
    double distance = DISTANCE_BASE;

};

#endif //CAMERAHANDLER_H
