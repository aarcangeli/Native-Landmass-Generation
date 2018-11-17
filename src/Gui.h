#ifndef GUI_H
#define GUI_H

class Application;

#include <SDL2/SDL.h>
#include <vector>
#include "LandmassGenerator.h"

struct nk_context;
struct nk_font_atlas;

const int SIDEBAR_WIDTH = 250;
const int TOPBAR_HEIGHT = 47;
const int STATUSBAR_HEIGHT = 22;

struct GuiPrivate;

class Gui {
    Application *app;
    bool isFirstEditor = true;
    nk_context *ctx;
    nk_font_atlas *atlas;
    int width, height;
    int editingLayer = -1;
    bool isGrabbingGuiLeft = false;
    bool isGrabbingLeft = false, isGrabbingMiddle = false, isGrabbingRight = false;
    LandmassParams defaultValues;
    LandmassParams lastValues;
    GuiPrivate *data;
    bool imagePopupActive = false;

public:
    void init(Application *app, SDL_Window *window);

    // wrappers of nk_*
    void inputBegin();
    int inputhandleEvent(SDL_Event &event);
    void inputEnd();
    void render();

    // custom editors
    void editor(LandmassParams &params);
    void terrainEditor(LandmassParams &params, TerrainType &type);

    void resize(int width, int height);
    void loadPalette(const LandmassParams &params);
};

#endif //GUI_H
