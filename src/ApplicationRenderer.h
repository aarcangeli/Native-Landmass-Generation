#ifndef APPLICATIONRENDERER_H
#define APPLICATIONRENDERER_H

class Application;

#include <memory>
#include "Shader.h"
#include "Mesh.h"
#include "Grid.h"
#include "ResourceLoader.h"

class ApplicationRenderer {
public:
    ApplicationRenderer();

    void init(Application *app);
    void resize(int width, int height);
    void updateMesh(const LandmassParams &params, const ChunkData &chunk);
    void render(const LandmassParams &params);

private:
    Application *app;
    Grid grid;
    std::shared_ptr<Shader> standardShader, unliteShader;
    Mesh mesh;
    int width, height;

};


#endif //APPLICATIONRENDERER_H
