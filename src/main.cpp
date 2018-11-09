#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <GL/glew.h>
#include "use_eigen.h"

#include <iostream>

#include "CameraHandler.h"
#include "PerlinNoise.h"
#include "Mesh.h"
#include "Shader.h"
#include "Gui.h"
#include "LandmassGenerator.h"
#include "ResourceLoader.h"
#include "ChunkData.h"
#include "math.h"
#include "res_glsl.h"
#include "res_img.h"

using namespace std;

SDL_Window *window;
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
bool wireframe = false;
ResourceLoader loader;
LandmassGenerator generator;
ChunkData chunk;
Shader mainShader;
Mesh mesh;

Texture checkerTexture;
LandmassParams params;

void resetDefaultParams();

void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar *message,
                const void *userParam) {
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}

bool InitGL() {
    if (glewInit() != GLEW_OK) {
        printf("ERROR: Cannot inizialize glew\n");
        return false;
    }

    if (!mainShader.compile(glsl::standard_vs, glsl::standard_fs)) {
        printf("ERROR: Cannot compile standard shader\n");
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

    // load textures
    params.texturePalette.push_back(loader.loadTexture(params, "water", img::water, img::water_end, GL_LINEAR, GL_LINEAR));
    params.texturePalette.push_back(loader.loadTexture(params, "sandyGrass", img::sandy_grass, img::sandy_grass_end, GL_LINEAR, GL_LINEAR));
    params.texturePalette.push_back(loader.loadTexture(params, "grass", img::grass, img::grass_end, GL_LINEAR, GL_LINEAR));
    params.texturePalette.push_back(loader.loadTexture(params, "rocks1", img::rocks1, img::rocks1_end, GL_LINEAR, GL_LINEAR));
    params.texturePalette.push_back(loader.loadTexture(params, "snow", img::snow, img::snow_end, GL_LINEAR, GL_LINEAR));

    checkerTexture = loader.loadTexture(params, "checker", img::checker, img::checker_end, GL_NEAREST, GL_NEAREST);

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);


    return true;
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
        generator.generateChunk(chunk, size, size, Region{0, 0, params.scale, params.scale});

        // generate the mesh
        bool useFastNormals = params.realtime != 0;
        chunk.updateMesh(mesh, useFastNormals);

        // update mesh
        mesh.bind();
        mesh.refresh();

        // update texture
        vector<float> textureData;
        switch (params.mode) {
            case DRAW_MODE_NOISE: {
                chunk.drawHeightMapTexture(textureData);
                break;
            }
            case DRAW_MODE_COLOURS:
                chunk.drawColorTexture(textureData);
                break;
        }
        //glBindTexture(GL_TEXTURE_2D, landmassTexture);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_FLOAT, textureData.data());
    }

    mainShader.bind();
    //mainShader.fillUniforms(params);
    mesh.bind();

    // update render settings
    //glUniform1fv(minHeightLocation, 1, (float *) &renderSettings);
    float array[] = {0, 0, 1, 1, 0, 0, 1, 1};

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
    //glDisable(GL_DEPTH_TEST);
    //glViewport(0, 0, 50, 50);
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
    if (!loader.init()) return 1;

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

    resetDefaultParams();

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

void resetDefaultParams() {
    params.layers.resize(0);
    params.layers.push_back(TerrainType{0, Color{1, 0, 0}, 0, 0, 0.5, 1});
    params.layers.push_back(TerrainType{1, Color{1, 0, 0}, 0.01, 0, 0.5, 1});
    params.layers.push_back(TerrainType{2, Color{1, 0, 0}, 0.02, 0, 0.5, 1});

    for (auto &it : params.layers) {
        it.directGlTexture = params.texturePalette[it.textureNumber].myTex;
    }
}
