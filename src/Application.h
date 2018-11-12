#ifndef APPLICATION_H
#define APPLICATION_H

#include <SDL.h>
#include <GL/glew.h>

#include "ResourceLoader.h"
#include "ApplicationRenderer.h"
#include "utils/CameraHandler.h"
#include "utils/PerlinNoise.h"
#include "Mesh.h"
#include "Shader.h"
#include "Gui.h"

class Application {
public:
    Application();

    int runLoop();

private:
    CameraHandler camera;
    SDL_Window *window;
    ResourceLoader loader;

    LandmassParams params;
    ApplicationRenderer renderer;
    LandmassGenerator generator;
    ChunkData chunk;

    void initGL();
    void updateState();

    int lastUpdate = 0;
    int lastFps = 0;
    int count = 0;

    void resetDefaultParams();
};


#endif //APPLICATION_H
