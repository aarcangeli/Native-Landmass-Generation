#ifndef APPLICATIONRENDERER_H
#define APPLICATIONRENDERER_H

#include "Shader.h"
#include "Mesh.h"

class ApplicationRenderer {
public:
    ApplicationRenderer();

    void init();
    void resize(int width, int height);
    void updateMesh(const LandmassParams &params, const ChunkData &chunk);
    void render(const LandmassParams &params);

private:
    Shader mainShader;
    Mesh mesh;
    int width, height;

};


#endif //APPLICATIONRENDERER_H
