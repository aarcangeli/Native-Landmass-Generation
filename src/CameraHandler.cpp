#include "CameraHandler.h"

#include <GL/gl.h>
#include "iostream"
#include <math.h>

using namespace std;

bool CameraHandler::handleEvent(SDL_Event &event) {
    if (event.type == SDL_MOUSEMOTION) {
        if (event.motion.state & SDL_BUTTON_LMASK) {
            rotationY += event.motion.xrel;
            rotationX += event.motion.yrel;
        }
    }
    if (event.type == SDL_MOUSEWHEEL) {
        if (event.wheel.y) {
            zoom -= event.wheel.y;
            distance = pow(2, zoom) * DISTANCE_BASE;
        }
    }
    return false;
}

void CameraHandler::applyViewMatrix() {
    glTranslated(0, 0, -distance);
    glRotated(rotationX, 1, 0, 0);
    glRotated(rotationY, 0, 1, 0);
}