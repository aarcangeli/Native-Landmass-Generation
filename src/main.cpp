#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <SDL_image.h>
#include <GL/glew.h>
#include "use_eigen.h"

#include <iostream>

#include "CameraHandler.h"
#include "PerlinNoise.h"
#include "Mesh.h"
#include "Shader.h"
#include "Gui.h"
#include "LandmassGenerator.h"
#include "ChunkData.h"
#include "math.h"
#include "res_glsl.h"
#include "res_img.h"

using namespace std;

SDL_Window *window;
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
bool wireframe = false;
LandmassGenerator generator;
ChunkData data;
Shader mainShader;
Mesh mesh;

GLuint landmassTexture;
LandmassParams params;

GLuint loadTextureFromRes(const char *name,
                          const char *start, const char *end,
                          GLint minFilter = GL_LINEAR,
                          GLint magFilter = GL_LINEAR) {
    GLuint ret;
    glGenTextures(1, &ret);
    glBindTexture(GL_TEXTURE_2D, ret);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    SDL_Surface *rawData = IMG_Load_RW(SDL_RWFromMem((void *) start, (int) (end - start)), 1);
    if (!rawData) {
        printf("ERROR: Cannot load %s\n", name);
        return 0;
    }
    SDL_Surface *grass = SDL_CreateRGBSurfaceWithFormat(0, rawData->w, rawData->h, 8, SDL_PIXELFORMAT_RGBA32);
    if (SDL_BlitSurface(rawData, nullptr, grass, nullptr) < 0) {
        printf("ERROR: Cannot load %s: %s\n", name, SDL_GetError());
        return 0;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, grass->w, grass->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, grass->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    return ret;
}

bool InitGL() {
    if (glewInit() != GLEW_OK) {
        printf("ERROR: Cannot inizialize glew\n");
        return false;
    }

    if (!mainShader.compile(glsl::standard_vs, glsl::standard_fs)) {
        printf("ERROR: Cannot compile phong shader\n");
        return false;
    }

    if (!mesh.init()) return false;
    mesh.setPositionAttribute(mainShader.getAttribLocation("position"));
    mesh.setNormalAttribute(mainShader.getAttribLocation("normal"));
    mesh.setTexCoordAttribute(mainShader.getAttribLocation("uv"));

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
    gluPerspective(45, (float) (WINDOW_WIDTH - SIDEBAR_WIDTH) / WINDOW_HEIGHT, 0.05, 1000);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glShadeModel(GL_SMOOTH);

    // Create a texture
    glGenTextures(1, &landmassTexture);
    glBindTexture(GL_TEXTURE_2D, landmassTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // load textures
    params.water.texture = loadTextureFromRes("water", img::water, img::water_end, GL_LINEAR, GL_LINEAR);
    params.sand.texture = loadTextureFromRes("sandy_grass", img::sandy_grass, img::sandy_grass_end, GL_LINEAR, GL_LINEAR);
    params.grass.texture = loadTextureFromRes("grass", img::grass, img::grass_end, GL_LINEAR, GL_LINEAR);
    params.rock.texture = loadTextureFromRes("rocks1", img::rocks1, img::rocks1_end, GL_LINEAR, GL_LINEAR);
    params.snow.texture = loadTextureFromRes("snow", img::snow, img::snow_end, GL_LINEAR, GL_LINEAR);

    return params.water.texture && params.sand.texture && params.grass.texture && params.rock.texture && params.snow.texture;
}

void render() {
    const int size = 128;
    const double meshSize = 3;
    bool refresh = false;

    // Update timer
    int now = SDL_GetTicks();
    static int lastUpdate = now;
    int delta = now - lastUpdate;
    lastUpdate = now;

    // show fps
    static int lastFps = now;
    static int count = 0;
    count++;
    if (now > lastFps + 500) {
        double deltaTime = (double) (now - lastFps) / count;
        string title = to_string(deltaTime) + "ms (" + to_string(1000.0 / deltaTime) + " fps)";
        SDL_SetWindowTitle(window, title.c_str());
        lastFps = now;
        count = 0;
    }

    // Setup OpenGL
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, WINDOW_WIDTH - SIDEBAR_WIDTH, WINDOW_HEIGHT);
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

    // refresh z param
    static LandmassParams lastParams;
    if (params.realtime) {
        params.z += delta / 1000.f;
        refresh = true;
    }
    if (params.refreshRequested) {
        params.z = rand() * 10;
        refresh = true;
    }
    if (lastParams != params) {
        lastParams = params;
        refresh = true;
    }
    params.refreshRequested = false;

    if (refresh) {
        // generate the chunk
        generator.configure(params);
        generator.generateChunk(data, size, size, Region{0, 0, params.scale, params.scale});

        // update texture
        glBindTexture(GL_TEXTURE_2D, landmassTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_FLOAT, data.textureData.data());

        // generate the mesh
        bool useFastNormals = params.realtime != 0;
        data.updateMesh(mesh, useFastNormals);

        // update mesh
        mesh.bind();
        mesh.refresh();
    }

    glBindTexture(GL_TEXTURE_2D, landmassTexture);
    mainShader.bind();
    mesh.bind();

    // Draw a cube
    glPushMatrix();
    {
        glTranslated(0, 0.06, 0);
        //
        glScaled(meshSize, meshSize, meshSize);
        glTranslated(-0.5, 0, -0.5);

        mesh.bind();
        mesh.draw();
    }
    glPopMatrix();

    mainShader.unbind();
    mesh.unbind();

    // Draw a little gizmo
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
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        cout << "Cannot load SDL_image: " << IMG_GetError() << endl;
        return 1;
    }

    window = SDL_CreateWindow("Native Landmass Generation", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);

    if (!window) {
        cout << "Could not create window: " << SDL_GetError() << endl;
        return 1;
    }

    SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);

    if (!InitGL()) return 1;
    Gui gui{window};
    gui.resize(WINDOW_WIDTH, WINDOW_HEIGHT);

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
