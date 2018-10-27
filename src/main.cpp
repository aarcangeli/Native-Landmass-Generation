#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <iostream>

#include "CameraHandler.h"
#include "PerlinNoise.h"

using namespace std;

const int WIDTH = 1280;
const int HEIGHT = 720;

GLuint texture;

// Generate a random texture tgb
void generateNoiseTexture(PerlinNoise &pn, float *textureData, int width, int height) {
    const int SCALE = 10;
    unsigned int kk = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            float x = (float) j / width;
            float y = (float) i / height;

            // calculate noise
            float n = (float) pn.noise(x * SCALE, y * SCALE, 0.8);
            textureData[kk + 0] = n;
            textureData[kk + 1] = n;
            textureData[kk + 2] = n;
            kk += 3;
        }
    }
}

void InitGL() {
    // Setup buffers
    glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(0.3, 0.3, 0.3, 1);
    glClearDepth(1);

    // Setup depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Setup projection camera matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float) WIDTH / HEIGHT, 0.5, 1000);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glShadeModel(GL_SMOOTH);

    // Create a texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
    const int size = 2;

    // update texture
    static int lastUpdate = -1;
    uint32_t now = SDL_GetTicks();
    if (lastUpdate < 0 || now - lastUpdate > 1000) {
        PerlinNoise pn{now};
        int width = 128, height = 128;
        float *textureData = new float[width * height * 3];
        generateNoiseTexture(pn, textureData, width, height);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, textureData);
        lastUpdate = now;
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

    while (true) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (camera.handleEvent(event)) continue;

            switch (event.type) {
                case SDL_QUIT:
                    SDL_Quit();
                    return 0;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            SDL_Quit();
                            return 0;
                    }
                    break;
            }

        }

        // setup camera
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        camera.applyViewMatrix();

        render();
        SDL_GL_SwapWindow(window);
    }

}
