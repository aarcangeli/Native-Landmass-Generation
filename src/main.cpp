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
    glShadeModel(GL_SMOOTH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float) WIDTH / HEIGHT, 0.1, 1000);
    glTranslated(0, 0, -5);
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

    // Draw a quad
    glBegin(GL_QUADS);
    glVertex2d(-1, -1);
    glVertex2d(-1, 1);
    glVertex2d(1, 1);
    glVertex2d(1, -1);
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
