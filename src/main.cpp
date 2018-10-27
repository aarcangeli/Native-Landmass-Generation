#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <iostream>

using namespace std;

const int WIDTH = 1280;
const int HEIGHT = 720;

void InitGL() {
    glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1);
    glShadeModel(GL_SMOOTH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float) WIDTH / HEIGHT, 0.1, 1000);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

    glTranslated(0, 0, -10);
    glRotated(45, 1, 0, 0);
    glRotated(45, 0, 1, 0);

    // Draw a "ground"
    drawGrid();

    double cyclePerSecond = 0.5;
    glRotated(SDL_GetTicks() / 1000. * cyclePerSecond * 360, 0, 1, 0);

    // Draw a cube
    glTranslated(0, 1, 0);

    glBegin(GL_QUADS);

    // front
    glColor3f(1, 1, 1);
    glVertex3d(-1, -1, 1);
    glVertex3d(-1, 1, 1);
    glVertex3d(1, 1, 1);
    glVertex3d(1, -1, 1);

    // right
    glColor3f(0, 0, 1);
    glVertex3d(1, -1, -1);
    glVertex3d(1, 1, -1);
    glVertex3d(1, 1, 1);
    glVertex3d(1, -1, 1);

    // left
    glColor3f(0, 1, 0);
    glVertex3d(-1, -1, -1);
    glVertex3d(-1, 1, -1);
    glVertex3d(-1, 1, 1);
    glVertex3d(-1, -1, 1);

    // back
    glColor3f(1, 1, 0);
    glVertex3d(-1, -1, -1);
    glVertex3d(-1, 1, -1);
    glVertex3d(1, 1, -1);
    glVertex3d(1, -1, -1);

    // top
    glColor3f(1, 0.5, 0);
    glVertex3d(-1, 1, -1);
    glVertex3d(1, 1, -1);
    glVertex3d(1, 1, 1);
    glVertex3d(-1, 1, 1);

    glEnd();
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Native Procedural Cave Generation", SDL_WINDOWPOS_CENTERED,
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

        render();
        SDL_GL_SwapWindow(window);
    }

}
