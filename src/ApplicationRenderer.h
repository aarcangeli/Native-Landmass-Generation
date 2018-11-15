#ifndef APPLICATIONRENDERER_H
#define APPLICATIONRENDERER_H

#include "Shader.h"
#include "Mesh.h"
#include "Grid.h"

class ApplicationRenderer {
public:
    ApplicationRenderer();

    void init();
    void resize(int width, int height);
    void updateMesh(const LandmassParams &params, const ChunkData &chunk);
    void render(const LandmassParams &params);

private:
    Grid grid;
    Shader mainShader, unliteShader;
    Mesh mesh;
    int width, height;

};


#endif //APPLICATIONRENDERER_H
