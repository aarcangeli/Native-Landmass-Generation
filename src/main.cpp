#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <iostream>

using namespace std;

const int WIDTH = 1280;
const int HEIGHT = 720;

void InitGL() {
    glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glShadeModel(GL_SMOOTH);
}

void render() {
    const double cyclePerSecond = 2;

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    for (int i = 0; i < 2; i++) {
        glLoadIdentity();
        glTranslated(-0.5 + i, 0, 0);
        glRotated(SDL_GetTicks() / 1000. * cyclePerSecond * 360, 0, 1, 0);

        glBegin(GL_TRIANGLE_FAN);
        glVertex2d(-0.9, -0.9);
        glVertex2d(0.9, -0.9);
        glVertex2d(0, 0.9);
        glEnd();
    }
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
