#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <iostream>

#include "CameraHandler.h"
#include "PerlinNoise.h"
#include "Gui.h"
#include "nlg.h"

using namespace std;

const int WIDTH = 1280;
const int HEIGHT = 720;

GLuint texture;

NoiseParams params;

// Generate a random texture tgb
void generateNoiseTexture(float *textureData, PerlinNoise &pn, int width, int height, float scale,
                          NoiseParams &params) {

    // get level range
    float level = 0;
    float amplitude = 1;
    for (int i = 0; i < params.octaves; ++i) {
        level += amplitude;
        amplitude *= params.persistence;
    }

    float z = params.z;
    unsigned int kk = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            float x = (float) j / width / scale;
            float y = (float) i / height / scale;

            float amplitude = 1;
            float frequency = 1;
            float noiseHeight = 0;

            for (int k = 0; k < params.octaves; k++) {
                float mapX = x * frequency + params.offsetX;
                float mapY = y * frequency + params.offsetY;
                // n is in range [-1,1]
                float n = (float) pn.noise(mapX, mapY, z) * 2 - 1;
                noiseHeight += n * amplitude;

                // advance
                amplitude *= params.persistence;
                frequency *= params.lacunarity;
            }

            // apply level transform
            noiseHeight = (noiseHeight + level) / (level * 2);

            // calculate noise
            if (params.mode == DRAW_MODE_COLOURS) {
                // get color
                for (auto it : params.types) {
                    if (noiseHeight <= it.height) {
                        textureData[kk + 0] = it.color.red;
                        textureData[kk + 1] = it.color.green;
                        textureData[kk + 2] = it.color.blue;
                        break;
                    }
                }
            } else {
                textureData[kk + 0] = noiseHeight;
                textureData[kk + 1] = noiseHeight;
                textureData[kk + 2] = noiseHeight;
            }
            kk += 3;
        }
    }
}

void InitGL() {
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    // Setup buffers
    glClearColor(0.3, 0.3, 0.3, 1);
    glClearDepth(1);

    // Setup depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Setup projection camera matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float) (WIDTH - SIDEBAR_WIDTH) / HEIGHT, 0.5, 1000);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glShadeModel(GL_SMOOTH);

    // Create a texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void drawGrid() {
    const int GRID_DIM = 10;
    const int GRID_DIVISIONS = 100;

    glPushAttrib(GL_CURRENT_BIT);

    glBegin(GL_LINES);
    glColor3d(0.5, 0.5, 0.5);
    float step = GRID_DIM * 2.f / GRID_DIVISIONS;
    for (int i = 0; i <= GRID_DIVISIONS; i++) {
        float current = GRID_DIM - i * step;
        glVertex3f(-GRID_DIM, 0, current);
        glVertex3f(GRID_DIM, 0, current);
        glVertex3f(current, 0, -GRID_DIM);
        glVertex3f(current, 0, GRID_DIM);
    }
    glEnd();
    glPopAttrib();
}

void render() {
    int now = SDL_GetTicks();
    static int lastUpdate = now;
    int delta = now - lastUpdate;
    lastUpdate = now;

    const int size = 2;

    glViewport(0, 0, WIDTH - SIDEBAR_WIDTH, HEIGHT);

    // update texture
    glBindTexture(GL_TEXTURE_2D, texture);

    if (params.refreshRequested) params.z = rand();
    if (params.realtime || params.refreshRequested) {
        static PerlinNoise pn;
        params.z += delta / 1000.f;

        int width = 128, height = 128;
        float *textureData = new float[width * height * 3];
        generateNoiseTexture(textureData, pn, width, height, 0.1, params);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, textureData);
        params.refreshRequested = false;
    }

    // Draw a "ground"
    glDisable(GL_TEXTURE_2D);
    drawGrid();

    // Draw a cube
    glTranslated(0, 0.06, 0);
    glColor3f(1, 1, 1);

    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    // top
    glTexCoord2f(0.0f, 0.0f);
    glVertex3d(-size, 0, -size);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3d(size, 0, -size);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3d(size, 0, size);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3d(-size, 0, size);

    glEnd();
}

int main() {
    CameraHandler camera;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Native Landmass Generation", SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);

    if (!window) {
        cout << "Could not create window: " << SDL_GetError() << endl;
        return 1;
    }

    SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);

    InitGL();
    Gui gui{window};
    gui.resize(WIDTH, HEIGHT);

    // init param
    params.types.emplace_back();
    params.types.back() = {0.4, {0, 0, 1}};
    params.types.emplace_back();
    params.types.back() = {1, {0, 1, 0}};

    while (true) {
        SDL_Event event;

        gui.inputBegin();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                SDL_Quit();
                return 0;
            }

            if (gui.inputhandleEvent(event)) continue;
            if (camera.handleEvent(event)) continue;
        }
        gui.inputEnd();

        // setup camera
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        camera.applyViewMatrix();

        render();

        gui.editor("Parameters", params);

        gui.render();

        SDL_GL_SwapWindow(window);
    }

}
