#include "CameraHandler.h"

#include <GL/glew.h>
#include "iostream"
#include <math.h>

using namespace std;

CameraHandler::CameraHandler() {
    transform = AngleAxisd(0, Vec3(1, 0, 0));
    transform.translate(Vec3(0, 0, -distance));
    transform.rotate(AngleAxisd(45 * M_PI / 180, Vec3(1, 0, 0)));
    transform.rotate(AngleAxisd(45 * M_PI / 180, Vec3(0, 1, 0)));
}

bool CameraHandler::handleEvent(SDL_Event &event) {
    if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
        switch (event.button.button) {
            case SDL_BUTTON_LEFT:
                state[STATE_MOUSE_LEFT] = event.button.state;
                setRelativeMouseMode();
                return true;
            case SDL_BUTTON_MIDDLE:
                state[STATE_MOUSE_MIDDLE] = event.button.state;
                setRelativeMouseMode();
                return true;
            case SDL_BUTTON_RIGHT:
                state[STATE_MOUSE_RIGHT] = event.button.state;
                setRelativeMouseMode();
                return true;
        }
    }
    if (event.type == SDL_MOUSEMOTION) {
        deltaX += event.motion.xrel;
        deltaY += event.motion.yrel;
    }
    if (event.type == SDL_MOUSEWHEEL) {
        if (event.wheel.y) {
            transform.pretranslate(Vec3(0, 0, distance));
            zoom -= event.wheel.y;
            distance = pow(1.2, zoom) * DISTANCE_BASE;
            transform.pretranslate(Vec3(0, 0, -distance));
            return true;
        }
    }
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
            case SDLK_LALT:
            case SDLK_RALT:
                state[STATE_KEY_ALT] = event.key.state;
                setRelativeMouseMode();
                return true;
            case SDLK_LCTRL:
            case SDLK_RCTRL:
                state[STATE_KEY_CTRL] = event.key.state;
                return true;
            case SDLK_LSHIFT:
            case SDLK_RSHIFT:
                state[STATE_KEY_SHIFT] = event.key.state;
                return true;
            case SDLK_SPACE:
                state[STATE_KEY_SPACE] = event.key.state;
                return true;
            case SDLK_w:
                state[STATE_KEY_W] = event.key.state;
                return shouldPreventWasd();
            case SDLK_a:
                state[STATE_KEY_A] = event.key.state;
                return shouldPreventWasd();
            case SDLK_s:
                state[STATE_KEY_S] = event.key.state;
                return shouldPreventWasd();
            case SDLK_d:
                state[STATE_KEY_D] = event.key.state;
                return shouldPreventWasd();
            case SDLK_q:
                state[STATE_KEY_Q] = event.key.state;
                return shouldPreventWasd();
            case SDLK_e:
                state[STATE_KEY_E] = event.key.state;
                return shouldPreventWasd();
            case SDLK_f:
                action = ACTION_FOCUS;
                return true;
        }
    }

    return false;
}

bool CameraHandler::shouldPreventWasd() const {
    return state[STATE_MOUSE_RIGHT];
}

void CameraHandler::setRelativeMouseMode() const {
    if ((state[STATE_KEY_ALT] && (state[STATE_MOUSE_LEFT] || state[STATE_MOUSE_MIDDLE])) || state[STATE_MOUSE_RIGHT]) {
        SDL_SetRelativeMouseMode(SDL_TRUE);
    } else {
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }
}

void CameraHandler::applyViewMatrix() {
    tick();

    double *pDouble = transform.data();
    glMultMatrixd(pDouble);
}

void CameraHandler::tick() {
    uint32_t now = SDL_GetTicks();
    if (!lastTicks) lastTicks = now;
    uint32_t delta = now - lastTicks;
    lastTicks = now;

    if (state[STATE_KEY_ALT]) {
        if (deltaX || deltaY) {
            if (state[STATE_KEY_SHIFT]) {
                deltaX *= 3;
                deltaY *= 3;
            }
            if (state[STATE_KEY_CTRL]) {
                deltaX /= 3;
                deltaY /= 3;
            }
            if (state[STATE_MOUSE_MIDDLE] || (state[STATE_MOUSE_LEFT] && state[STATE_MOUSE_RIGHT])) {
                transform.pretranslate(Vec3(deltaX * STEP_PAN, 0, 0));
                transform.pretranslate(Vec3(0, -deltaY * STEP_PAN, 0));
            } else if (state[STATE_MOUSE_LEFT]) {
                transform.pretranslate(Vec3(0, 0, distance));
                auto top = transform.rotation() * Vec3(0, 1, 0);
                transform.prerotate(AngleAxisd(deltaX * STEP_ROT, top));
                transform.prerotate(AngleAxisd(deltaY * STEP_ROT, Vec3(1, 0, 0)));
                transform.pretranslate(Vec3(0, 0, -distance));
            } else if (state[STATE_MOUSE_RIGHT]) {
                transform.pretranslate(Vec3(0, 0, distance));
                zoom -= deltaX / 100.;
                distance = pow(1.2, zoom) * DISTANCE_BASE;
                transform.pretranslate(Vec3(0, 0, -distance));
            }
        }

    } else {
        if (state[STATE_MOUSE_RIGHT]) {
            double step = delta;
            if (state[STATE_KEY_CTRL]) step /= 3;
            if (state[STATE_KEY_SHIFT]) step *= 3;
            if (state[STATE_KEY_W]) transform.pretranslate(Vec3(0, 0, step * STEP_MOVE));
            if (state[STATE_KEY_S]) transform.pretranslate(Vec3(0, 0, -step * STEP_MOVE));
            if (state[STATE_KEY_A]) transform.pretranslate(Vec3(step * STEP_MOVE, 0, 0));
            if (state[STATE_KEY_D]) transform.pretranslate(Vec3(-step * STEP_MOVE, 0, 0));
            if (state[STATE_KEY_Q]) transform.pretranslate(Vec3(0, step * STEP_MOVE, 0));
            if (state[STATE_KEY_E]) transform.pretranslate(Vec3(0, -step * STEP_MOVE, 0));
            if (deltaX || deltaY) {
                auto top = transform.rotation() * Vec3(0, 1, 0);
                transform.prerotate(AngleAxisd(deltaX * STEP_ROT, top));
                transform.prerotate(AngleAxisd(deltaY * STEP_ROT, Vec3(1, 0, 0)));
            }
        }
    }
    if (state[STATE_MOUSE_LEFT] || state[STATE_MOUSE_MIDDLE] || state[STATE_MOUSE_RIGHT]) {
        action = ACTION_NOTHING;
    }
    if (action) {
        switch (action) {
            case ACTION_FOCUS:
                transform.translation() = Vec3(0, 0, 0);
                transform.pretranslate(Vec3(0, 0, -distance));
                break;
        }
        action = ACTION_NOTHING;
    }

    deltaX = deltaY = 0;
}
