#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "use_eigen.h"

#include <iostream>

#include "CameraHandler.h"
#include "PerlinNoise.h"
#include "Mesh.h"
#include "Shader.h"
#include "Gui.h"
#include "nlg.h"
#include "math.h"
#include "res_glsl.h"

using namespace std;

SDL_Window *window;
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
bool wireframe = false;
Shader mainShader;
Mesh mesh;

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
    gluPerspective(45, (float) (WINDOW_WIDTH - SIDEBAR_WIDTH) / WINDOW_HEIGHT, 0.5, 1000);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glShadeModel(GL_SMOOTH);

    // Create a texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return true;
}

void updateMesh(const float *noiseData, int width, int height, NoiseParams &params) {
    // every cell of the matrix is mappet into a vertex
    // so the number of vertices is the same as the number of items in noiseData
    uint32_t numberOfVertices = (uint32_t) (width * height);

    // each face has a vertex apart the last row and the last column
    uint32_t numberOfFaces = numberOfVertices - width - height + 1;

    mesh.resize(numberOfVertices, numberOfFaces);

    // fill the vertices
    uint32_t ii = 0;
    auto &vertices = mesh.vertices;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            auto &v = vertices[ii];
            v.position = {(float) x / width, noiseData[ii], (float) y / width};
            v.textCoord[0] = (float) x / width;
            v.textCoord[1] = (float) y / width;

            glTexCoord2f((float) x / width, (float) y / height);

            ii++;
        }
    }

    // fast normal calculation
    if (params.realtime) {
        ii = 0;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                auto &v1 = vertices[ii];
                if (x == 0 || y == 0) {
                    v1.normal = {0, 1, 0};
                } else {
                    auto &u = vertices[ii - width].position - v1.position;
                    auto &v = vertices[ii - 1].position - v1.position;
                    v1.normal = {
                            (u[1] * v[2]) - (u[2] * v[1]),
                            (u[2] * v[0]) - (u[0] * v[2]),
                            (u[0] * v[1]) - (u[1] * v[0]),
                    };
                }
                ii++;
            }
        }
    }

    // fill the indexes
    auto *face = &mesh.faces.front();
    for (uint32_t y = 1; y < height; y++) {
        for (uint32_t x = 1; x < width; x++) {
            face->vertex1 = (x - 1) + (y - 1) * width;
            face->vertex2 = (x - 1) + y * width;
            face->vertex3 = x + y * width;
            face->vertex4 = x + (y - 1) * width;
            face++;
        }
    }

    // this method is too expensive for real time
    if (!params.realtime) {
        mesh.calculateNormals();
    }
    mesh.bind();
    mesh.refresh();
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
    static NoiseParams lastParams;
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
        // generate noise map
        static float *noiseData = new float[size * size];
        static PerlinNoise pn;
        generateNoiseMap(noiseData, pn, size, size, params);

        // convert noise map to texture
        float *textureData = new float[size * size * 3];
        convertNoiseMapToTexture(textureData, noiseData, size, size, params);

        // apply curve
        int ii = 0;
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                float &it = noiseData[ii++];
                it = (float) pow(10, it);
                it = it * params.heightMultiplier;
            }
        }

        // update mesh
        updateMesh(noiseData, size, size, params);

        // update texture
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size, size, 0, GL_RGB, GL_FLOAT, textureData);
    }

    glBindTexture(GL_TEXTURE_2D, texture);
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
