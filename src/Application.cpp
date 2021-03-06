#include "Application.h"

#include "utils/gl-utils.h"
#include "utils/use-eigen.h"
#include "res_img.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const int MESH_SIZE = 128;

using namespace std;

Application::Application() = default;

int Application::runLoop() {

    SDL_Init(SDL_INIT_VIDEO);
    loader.init();

    window = SDL_CreateWindow("Native Landmass Generation", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!window) {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    initGL();
    gui.init(this, window);
    gui.resize(WINDOW_WIDTH, WINDOW_HEIGHT);
    renderer.resize(WINDOW_WIDTH, WINDOW_HEIGHT);
    chunk.resize(MESH_SIZE, MESH_SIZE);

    resetDefaultParams();

    while (true) {
        SDL_Event event;

        gui.inputBegin();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                SDL_Quit();
                return 0;
            }
            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    int width = event.window.data1;
                    int height = event.window.data2;
                    gui.resize(width, height);
                    renderer.resize(width, height);
                }
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

        camera.tick();

        // setup camera
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        updateState();

        renderer.render(params);

        gui.editor(params);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glActiveTexture(GL_TEXTURE0);
        gui.render();

        SDL_GL_SwapWindow(window);
    }
}

void Application::initGL() {
    SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    if (glewInit() != GLEW_OK) {
        printf("ERROR: Cannot inizialize glew\n");
        throw;
    }

    // Setup buffers
    glClearColor(0.3, 0.3, 0.3, 1);
    glClearDepth(1);

    // Setup depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glShadeModel(GL_SMOOTH);

    // load textures
    params.texturePalette.push_back(loader.loadTexture("water", img::water, img::water_end, GL_LINEAR, GL_LINEAR));
    params.texturePalette.push_back(loader.loadTexture("sandyGrass", img::sandy_grass, img::sandy_grass_end, GL_LINEAR, GL_LINEAR));
    params.texturePalette.push_back(loader.loadTexture("grass", img::grass, img::grass_end, GL_LINEAR, GL_LINEAR));
    params.texturePalette.push_back(loader.loadTexture("rocks1", img::rocks1, img::rocks1_end, GL_LINEAR, GL_LINEAR));
    params.texturePalette.push_back(loader.loadTexture("snow", img::snow, img::snow_end, GL_LINEAR, GL_LINEAR));

    // During init, enable debug output
    installGlLogger();

    renderer.init(this);
}

void Application::resetDefaultParams() {
    params.layers.resize(0);
    params.layers.emplace_back("Water", 0, 0.05, 0.02, 1);
    params.layers.emplace_back("Sand", 1, 0.08, 0.02, 1);
    params.layers.emplace_back("Grass", 2, 0.14, 0.02, 1);
    params.layers.emplace_back("Rocks", 3, 0.25, 0.02, 1);
    params.layers.emplace_back("Snow", 4, 100, 0.02, 10);

    for (auto &it : params.layers) {
        it.directGlTexture = params.texturePalette[it.textureNumber].myTex;
    }
}

void Application::updateState() {
    // Update timer
    int now = SDL_GetTicks();
    if (!lastUpdate) lastUpdate = now;
    int delta = now - lastUpdate;
    lastUpdate = now;

    // show fps
    if (!lastFps) lastFps = now;
    count++;
    if (now > lastFps + 500) {
        double deltaTime = (double) (now - lastFps) / count;
        string title = to_string(deltaTime) + "ms (" + to_string(1000.0 / deltaTime) + " fps)";
        SDL_SetWindowTitle(window, title.c_str());
        lastFps = now;
        count = 0;
    }

    if (now - lastDigest > 1000) {
        loader.digest();
        lastDigest = now;
    }

    // refresh z param
    bool refresh = false;
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
        generator.generateChunk(chunk, Region{0, 0, params.scale, params.scale});

        // generate the mesh
        renderer.updateMesh(params, chunk);
    }
}

ResourceLoader *Application::getLoader() {
    return &loader;
}
