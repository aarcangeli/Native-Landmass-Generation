#include "CameraHandler.h"

#include <GL/glew.h>
#include "iostream"
#include <math.h>

using namespace std;

bool CameraHandler::handleEvent(SDL_Event &event) {
    if (event.type == SDL_MOUSEMOTION) {
        if (event.motion.state & SDL_BUTTON_LMASK) {
            rotationY += event.motion.xrel;
            rotationX += event.motion.yrel;
            return true;
        }
    }
    if (event.type == SDL_MOUSEWHEEL) {
        if (event.wheel.y) {
            zoom -= event.wheel.y;
            distance = pow(2, zoom) * DISTANCE_BASE;
            return true;
        }
    }
    return false;
}

void CameraHandler::applyViewMatrix() {
    glTranslated(0, 0, -distance);
    glRotated(rotationX, 1, 0, 0);
    glRotated(rotationY, 0, 1, 0);
}
