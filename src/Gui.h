#ifndef GUI_H
#define GUI_H

#include <SDL2/SDL.h>
#include "landmass.h"

struct nk_context;
struct nk_font_atlas;

const int SIDEBAR_WIDTH = 250;

class Gui {
    bool isFirstEditor = true;
    nk_context *ctx;
    nk_font_atlas *atlas;
    int width, height;
    int changingType = -1;
    bool isGrabbingGui = false;
    bool isGrabbingFrame = false;
    LandmassParams defaultValues;
    LandmassParams lastValues;

public:
    Gui(SDL_Window *window);

    // wrappers of nk_*
    void inputBegin();
    int inputhandleEvent(SDL_Event &event);
    void inputEnd();
    void render();

    // custom editors
    void editor(const char *string, LandmassParams &params);

    void resize(const int width, const int height);

    void evaluateInsideAWindow();
};

#endif //GUI_H
