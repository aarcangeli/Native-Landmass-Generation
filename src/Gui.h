#ifndef GUI_H
#define GUI_H

#include <SDL2/SDL.h>
#include "nlg.h"

struct nk_context;
struct nk_font_atlas;

const int SIDEBAR_WIDTH = 250;

class Gui {
    nk_context *ctx;
    nk_font_atlas *atlas;
    int mouseX, mouseY;
    bool isInsideAWindow;
    int width, height;
    int changingType = -1;

public:
    Gui(SDL_Window *window);

    // wrappers of nk_*
    void inputBegin();
    int inputhandleEvent(SDL_Event &event);
    void inputEnd();
    void render();

    // custom editors
    void editor(const char *string, NoiseParams &params);

    void resize(const int width, const int height);

    void evaluateInsideAWindow();
};

#endif //GUI_H
