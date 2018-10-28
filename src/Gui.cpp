#include "Gui.h"
#include "iostream"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_GL2_IMPLEMENTATION
#define NK_BUTTON_TRIGGER_ON_RELEASE

#include "nuklear.h"
#include "nuklear_sdl_gl2.h"

Gui::Gui(SDL_Window *window) {
    ctx = nk_sdl_init(window);
    nk_sdl_font_stash_begin(&atlas);
    nk_sdl_font_stash_end();
}

void Gui::inputBegin() {
    nk_input_begin(ctx);
    SDL_CaptureMouse(SDL_TRUE);
}

int Gui::inputhandleEvent(SDL_Event &event) {
    // workaround mouse disappear
    ctx->input.mouse.grab = 0;
    ctx->input.mouse.ungrab = 0;
    ctx->input.mouse.grabbed = 0;

    bool isAnyActive = nk_item_is_any_active(ctx) != 0;

    if (!isAnyActive && event.type == SDL_MOUSEWHEEL) {
        // wheel should not be grabbed by window
        return false;
    }

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        isGrabbingGui = isAnyActive;
        isGrabbingFrame = !isAnyActive;
    }

    if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
        isGrabbingGui = false;
        isGrabbingFrame = false;
    }

    // when a window has focus capture all keyboard
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP || event.type == SDL_TEXTINPUT) {
        if (isAnyActive) nk_sdl_handle_event(&event);
        return isAnyActive;
    }

    // mark input as grabbed just if mouse is inside a window
    return (nk_sdl_handle_event(&event) && isAnyActive && !isGrabbingFrame) || isGrabbingGui;
}

void Gui::inputEnd() {
    nk_input_end(ctx);
}

void Gui::render() {
    nk_sdl_render(NK_ANTI_ALIASING_ON);
}

void Gui::resize(int _width, int _height) {
    width = _width;
    height = _height;
}

void Gui::editor(const char *string, NoiseParams &params) {
    int windowWidth = SIDEBAR_WIDTH;
    int windowHeight = height;
    struct nk_rect bounds = nk_rect(width - windowWidth, 0, windowWidth, windowHeight);
    static NoiseParams defaultValues = params;

    if (nk_begin(ctx, "Noise", bounds, NK_WINDOW_BORDER | NK_WINDOW_MOVABLE)) {
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, "View:", NK_TEXT_LEFT);
        nk_radio_label(ctx, "Real Time", &params.realtime);
        nk_label(ctx, "Mode:", NK_TEXT_LEFT);
        if (nk_option_label(ctx, "Noise Map", params.mode == DRAW_MODE_NOISE)) params.mode = DRAW_MODE_NOISE;
        if (nk_option_label(ctx, "Colour Map", params.mode == DRAW_MODE_COLOURS)) params.mode = DRAW_MODE_COLOURS;
        nk_property_float(ctx, "Scale:", 0.01, &params.scale, 100, 0.01, 0.01);
        nk_property_float(ctx, "Height:", 0.01, &params.heightMultiplier, 10, 0.01, 0.01);

        nk_label(ctx, "Noise:", NK_TEXT_LEFT);
        nk_property_int(ctx, "Octaves:", 1, &params.octaves, 10, 1, 1);
        nk_property_float(ctx, "Persistence:", 0, &params.persistence, 1, 0.01, 0.01);
        nk_property_float(ctx, "Lacunarity:", 0, &params.lacunarity, 10, 0.05, 0.05);
        nk_layout_row_dynamic(ctx, 30, 3);
        nk_label(ctx, "Offset:", NK_TEXT_LEFT);
        nk_property_float(ctx, "X:", -100, &params.offsetX, 100, 0.1, 0.1);
        nk_property_float(ctx, "Y:", -100, &params.offsetY, 100, 0.1, 0.1);
        nk_layout_row_dynamic(ctx, 30, 1);
        if (nk_button_label(ctx, "Defaults")) {
            params.octaves = defaultValues.octaves;
            params.persistence = defaultValues.persistence;
            params.lacunarity = defaultValues.lacunarity;
            params.offsetX = defaultValues.offsetX;
            params.offsetY = defaultValues.offsetY;
        }
        if (!params.realtime) {
            if (nk_button_label(ctx, "Generate")) params.refreshRequested = 1;
        }

        nk_label(ctx, "Terrain types:", NK_TEXT_LEFT);
        int size = params.types.size();
        nk_property_int(ctx, "Size:", 0, &size, 10, 1, 1);
        params.types.resize(size);
        for (int i = 0; i < size; i++) {
            TerrainType &type = params.types[i];
            if (nk_button_label(ctx, type.name.c_str())) {
                changingType = i;
            }
        }
    }
    nk_end(ctx);

    if (changingType >= params.types.size()) {
        changingType = -1;
    }
    if (changingType >= 0) {
        TerrainType &type = params.types[changingType];
        if (nk_begin(ctx, "Color", nk_rect(50, 50, 250, 400), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE)) {
            // name
            nk_layout_row_dynamic(ctx, 30, 1);
            char name[512];
            strcpy(name, type.name.c_str());
            nk_edit_string_zero_terminated(ctx, NK_EDIT_SIMPLE, name, sizeof(name), nk_filter_ascii);
            type.name = name;

            // height
            nk_property_float(ctx, "#Height:", 0, &type.height, 1, 0.01, 0.01);

            // color
            nk_layout_row_dynamic(ctx, 120, 1);
            nk_colorf bg{type.color.red, type.color.green, type.color.blue};
            bg = nk_color_picker(ctx, bg, NK_RGB);
            nk_layout_row_dynamic(ctx, 25, 1);
            bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f, 0.005f);
            bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f, 0.005f);
            bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f, 0.005f);
            type.color = {bg.r, bg.g, bg.b};

            if (nk_button_label(ctx, "Close")) changingType = -1;
        }
        nk_end(ctx);
    }
}
