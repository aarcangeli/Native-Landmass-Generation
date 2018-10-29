#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include <iostream>

#include "CameraHandler.h"
#include "PerlinNoise.h"
#include "Shader.h"
#include "Gui.h"
#include "nlg.h"
#include "math.h"
#include "res_glsl.h"

using namespace std;

const int WIDTH = 1280;
const int HEIGHT = 720;
bool wireframe = false;
Shader mainShader;

GLuint texture;

NoiseParams params;

void generateNoiseMap(float *noiseData, PerlinNoise &pn, int width, int height, NoiseParams &params) {
    float scale = params.scale;
    float z = params.z;
    unsigned int ii = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            float x = (j - width / 2.f) / width / scale;
            float y = (i - height / 2.f) / height / scale;

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

            // calculate noise
            noiseData[ii++] = noiseHeight;
        }
    }
}

void convertNoiseMapToTexture(float *textureData, float *noiseData, int width, int height, NoiseParams &params) {
    // get level range
    float level = 0;
    float amplitude = 1;
    for (int i = 0; i < params.octaves; ++i) {
        level += amplitude;
        amplitude *= params.persistence;
    }
    level = level * 0.7f;

    unsigned int ii = 0;
    unsigned int kk = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            float noiseHeight = noiseData[ii++];

            // apply level transform
            noiseHeight = (noiseHeight + level) / (level * 2);

            // calculate noise
            if (params.mode == DRAW_MODE_COLOURS) {
                // get color
                textureData[kk + 0] = 0;
                textureData[kk + 1] = 0;
                textureData[kk + 2] = 0;
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

bool InitGL() {
    if (!mainShader.compile(glsl::standard_vs, glsl::standard_fs)) {
        printf("ERROR: Cannot compile phong shader\n");
        return false;
    }

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

    return true;
}

void render() {
    const int width = 128, height = 128;
    const double meshSize = 3;

    // Update timer
    int now = SDL_GetTicks();
    static int lastUpdate = now;
    int delta = now - lastUpdate;
    lastUpdate = now;

    // Setup OpenGL
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, WIDTH - SIDEBAR_WIDTH, HEIGHT);
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

    // refresh z param
    if (params.refreshRequested) params.z = rand() * 10;
    if (params.realtime) params.z += delta / 1000.f;
    params.refreshRequested = false;

    // generate noise map
    float *noiseData = new float[width * height];
    static PerlinNoise pn;
    generateNoiseMap(noiseData, pn, width, height, params);

    // convert noise map to texture
    float *textureData = new float[width * height * 3];
    convertNoiseMapToTexture(textureData, noiseData, width, height, params);

    // update texture
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, textureData);

    // apply curve
    int ii = 0;
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            float &it = noiseData[ii++];
            it = (float) pow(10, it);
        }
    }

    mainShader.bind();

    // Draw a cube
    glPushMatrix();
    {
        glTranslated(0, 0.06, 0);
        glTranslated(-meshSize / 2, 0, -meshSize / 2);
        glColor3f(1, 1, 1);
        glScalef(1, params.heightMultiplier * params.scale, 1);

        glEnable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);

        for (int y = 0; y < height - 1; y++) {
            for (int x = 0; x < width - 1; x++) {

                glTexCoord2f((float) x / width, (float) y / height);
                glVertex3d(x * meshSize / width,
                           noiseData[x + y * width],
                           y * meshSize / height);

                glTexCoord2f((float) x / width, (float) (y + 1) / height);
                glVertex3d((x + 1) * meshSize / width,
                           noiseData[(x + 1) + y * width],
                           y * meshSize / height);

                glTexCoord2f((float) (x + 1) / width, (float) (y + 1) / height);
                glVertex3d((x + 1) * meshSize / width,
                           noiseData[(x + 1) + (y + 1) * width],
                           (y + 1) * meshSize / height);

                glTexCoord2f((float) (x + 1) / width, (float) y / height);
                glVertex3d(x * meshSize / width,
                           noiseData[x + (y + 1) * width],
                           (y + 1) * meshSize / height);
            }
        }

        glEnd();
    }
    glPopMatrix();

    mainShader.unbind();

    // Draw a little gizmo
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, 50, 50);
    glBegin(GL_LINES);

    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(2, 0, 0);

    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 2, 0);

    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 2);

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
    glewInit();

    if (!InitGL()) return 1;
    Gui gui{window};
    gui.resize(WIDTH, HEIGHT);

    // init param
    params.types.emplace_back();
    params.types.back() = {"Deep Water", 0.3, {52 / 255.f, 98 / 255.f, 195 / 255.f}};
    params.types.emplace_back();
    params.types.back() = {"Water", 0.4, {54 / 255.f, 102 / 255.f, 199 / 255.f}};
    params.types.emplace_back();
    params.types.back() = {"Sand", 0.45, {211 / 255.f, 206 / 255.f, 126 / 255.f}};
    params.types.emplace_back();
    params.types.back() = {"Grass", 0.6, {63 / 255.f, 106 / 255.f, 19 / 255.f}};
    params.types.emplace_back();
    params.types.back() = {"Grass 2", 0.7, {94 / 255.f, 68 / 255.f, 64 / 255.f}};
    params.types.emplace_back();
    params.types.back() = {"Rock", 0.8, {75 / 255.f, 60 / 255.f, 57 / 255.f}};
    params.types.emplace_back();
    params.types.back() = {"Snow", 1, {1, 1, 1}};

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

            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                        wireframe = !wireframe;
                        continue;
                }
            }
        }
        gui.inputEnd();

        // setup camera
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        camera.applyViewMatrix();

        render();

        gui.editor("Parameters", params);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        gui.render();

        SDL_GL_SwapWindow(window);
    }

}
