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

struct GuiPrivate {
    std::vector<struct nk_image> textures;
    nk_style_button style;
};

Gui::Gui(SDL_Window *window) {
    ctx = nk_sdl_init(window);
    nk_sdl_font_stash_begin(&atlas);
    nk_sdl_font_stash_end();
    data = new GuiPrivate;

    ctx->style.window.border = 2;

    nk_style_button &style = data->style;
    style = ctx->style.button;
    style.normal.data.color = {80, 80, 80, 255};
    style.hover.data.color = {90, 90, 90, 255};
    style.border_color = {80, 80, 80, 255};
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

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) isGrabbingGuiLeft = isAnyActive;
        if (event.button.button == SDL_BUTTON_LEFT) isGrabbingLeft = !isAnyActive;
        if (event.button.button == SDL_BUTTON_MIDDLE) isGrabbingMiddle = !isAnyActive;
        if (event.button.button == SDL_BUTTON_RIGHT) isGrabbingRight = !isAnyActive;
    }

    if (event.type == SDL_MOUSEBUTTONUP) {
        if (event.button.button == SDL_BUTTON_LEFT) isGrabbingGuiLeft = false;
        if (event.button.button == SDL_BUTTON_LEFT) isGrabbingLeft = false;
        if (event.button.button == SDL_BUTTON_MIDDLE) isGrabbingMiddle = false;
        if (event.button.button == SDL_BUTTON_RIGHT) isGrabbingRight = false;
    }

    if (isGrabbingLeft || isGrabbingMiddle || isGrabbingRight) return false;

    // when a window has focus capture all keyboard
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP || event.type == SDL_TEXTINPUT) {
        if (isAnyActive) nk_sdl_handle_event(&event);
        return isAnyActive;
    }

    // mark input as grabbed just if mouse is inside a window
    return (nk_sdl_handle_event(&event) && isAnyActive) || isGrabbingGuiLeft;
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

bool sortTerrainByHeight(TerrainType &a, TerrainType &b) {
    return a.height < b.height;
}

void Gui::editor(LandmassParams &params) {
    if (isFirstEditor) {
        defaultValues = params;
        lastValues = params;
        loadPalette(params);
    }

    int windowWidth = SIDEBAR_WIDTH;
    int windowHeight = height;
    struct nk_rect bounds = nk_rect(width - windowWidth, 0, windowWidth, windowHeight);

    if (nk_begin(ctx, "Noise", bounds, NK_WINDOW_BORDER | NK_WINDOW_MOVABLE)) {
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, "View:", NK_TEXT_LEFT);
        nk_radio_label(ctx, "Real Time", &params.realtime);
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

        nk_label(ctx, "Layers:", NK_TEXT_LEFT);
        int size = (int) params.layers.size();
        nk_property_int(ctx, "Count:", 1, &size, 20, 1, 1);
        params.layers.resize((unsigned) size);
        if (editingLayer >= size) {
            editingLayer = -1;
        }
        for (int i = 0; i < size; ++i) {
            if (i < size - 1 && params.layers[i].height > params.layers[i + 1].height) {
                TerrainType temp = params.layers[i];
                params.layers[i] = params.layers[i + 1];
                params.layers[i + 1] = temp;
                if (editingLayer == i) editingLayer++;
                else if (editingLayer == i + 1) editingLayer--;
            }
            TerrainType &layer = params.layers[i];
            std::string name = layer.name + "(" + std::to_string(layer.height) + ")";
            if (nk_button_label(ctx, name.c_str())) editingLayer = i;
        }
    }
    nk_end(ctx);

    if (editingLayer >= 0) {
        terrainEditor(params, params.layers[editingLayer]);
    }

    const char *toolTip = "";
    struct nk_style_window old = ctx->style.window;
    ctx->style.window.padding = {10, 12};
    ctx->style.window.spacing = {8, 10};
    if (nk_begin(ctx, "Buttons", nk_rect(0, 0, width - SIDEBAR_WIDTH, TOPBAR_HEIGHT), NK_WINDOW_NO_SCROLLBAR)) {
        nk_layout_row_static(ctx, TOPBAR_HEIGHT - 24, 33, 10);
        if (nk_widget_is_hovered(ctx)) toolTip = "Wireframe (W)";
        if (nk_button_label_styled(ctx, &data->style, "W")) params.wireframe = !params.wireframe;
//        if (nk_widget_is_hovered(ctx)) toolTip = "Select (S)";
//        nk_button_label_styled(ctx, &data->style, "B2");
//        if (nk_widget_is_hovered(ctx)) toolTip = "Help (F1)";
//        nk_button_label_styled(ctx, &data->style, "?");
    }
    ctx->style.window = old;
    nk_end(ctx);

    if (nk_begin(ctx, "Status", nk_rect(0, height - STATUSBAR_HEIGHT, width - SIDEBAR_WIDTH, STATUSBAR_HEIGHT), NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_NO_INPUT)) {
        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, toolTip, NK_TEXT_LEFT);
    }
    nk_end(ctx);

    isFirstEditor = false;
}

void Gui::terrainEditor(LandmassParams &params, TerrainType &type) {
    std::vector<struct nk_image> &textures = data->textures;
    struct nk_rect bounds = nk_rect(0, TOPBAR_HEIGHT, 400, 500);

    if (nk_begin(ctx, "Layer", bounds, NK_WINDOW_BORDER | NK_WINDOW_MOVABLE)) {
        nk_layout_row_dynamic(ctx, 0, 1);

        // name
        char name[512];
        strcpy(name, type.name.c_str());
        nk_edit_string_zero_terminated(ctx, NK_EDIT_SIMPLE, name, sizeof(name), nk_filter_ascii);
        type.name = name;

        // texture
        int width = SIDEBAR_WIDTH / 2;
        nk_layout_row_static(ctx, width, width, 3);
        nk_label(ctx, "Texture:", NK_TEXT_LEFT);
        if (nk_button_image(ctx, textures[type.textureNumber])) {
            imagePopupActive = !imagePopupActive;
        }

        if (imagePopupActive) {
            if (nk_popup_begin(ctx, NK_POPUP_DYNAMIC, "Image Popup", 0, nk_rect(265, 0, 275, 220))) {
                nk_layout_row_static(ctx, 80, 80, 3);
                int i = 0;
                for (auto &it : textures) {
                    if (nk_button_image(ctx, it)) {
                        type.textureNumber = i;
                        type.directGlTexture = params.texturePalette[i].myTex;
                        imagePopupActive = false;
                        nk_popup_close(ctx);
                    }
                    i++;
                }
                nk_popup_end(ctx);
            }
        }

        // height
        nk_layout_row_dynamic(ctx, 0, 1);
        nk_spacing(ctx, 1);
        nk_property_float(ctx, "#Height:", 0, &type.height, 1, 0.01, 0.01);
        nk_property_float(ctx, "#Blend:", 0, &type.blend, 1, 0.01, 0.01);
        nk_property_float(ctx, "#Texture scale:", 0, &type.textureScale, 1, 0.01, 0.01);

        nk_layout_row_dynamic(ctx, 0, 1);
        nk_spacing(ctx, 1);
        if (nk_button_label(ctx, "Close")) editingLayer = -1;
    }
    nk_end(ctx);
}

void Gui::loadPalette(const LandmassParams &params) {
    std::vector<struct nk_image> &textures = data->textures;
    textures.resize(params.texturePalette.size());
    int i = 0;
    for (auto &it : params.texturePalette) {
        textures[i] = nk_image_id(it.myTex);
        i++;
    }
}
