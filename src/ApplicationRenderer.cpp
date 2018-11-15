#include "ApplicationRenderer.h"

#include <SDL.h>
#include <string>

#include "res_glsl.h"
#include "Gui.h"

ApplicationRenderer::ApplicationRenderer() = default;

void ApplicationRenderer::init() {
    if (!mainShader.compile(glsl::standard_vs, glsl::standard_fs)) {
        printf("ERROR: Cannot compile standard shader\n");
        throw;
    }
    if (!unliteShader.compile(glsl::unlite_vs, glsl::unlite_fs)) {
        printf("ERROR: Cannot compile unlite shader\n");
        throw;
    }

    mesh.init();
    mesh.setPositionAttribute(mainShader.getAttribLocation("position"));
    mesh.setNormalAttribute(mainShader.getAttribLocation("normal"));
    mesh.setTexCoordAttribute(mainShader.getAttribLocation("uv"));

    grid.init();
    grid.generateAxes(5);
    grid.generateGrid(11, 11, 10, {.4f, .4f, .4f});
    grid.bind();
    grid.refresh();
    grid.setPositionAttribute(unliteShader.getAttribLocation("position"));
    grid.setColorAttribute(unliteShader.getAttribLocation("color"));
}

void ApplicationRenderer::updateMesh(const LandmassParams &params, const ChunkData &chunk) {
    bool useFastNormals = params.realtime != 0;
    chunk.updateMesh(mesh, useFastNormals);

    // update mesh
    mesh.bind();
    mesh.refresh();
}

void ApplicationRenderer::render(const LandmassParams &params) {
    const double meshSize = 2;

    // Setup OpenGL
    glEnable(GL_DEPTH_TEST);
    int canvasWidth = width - SIDEBAR_WIDTH;
    int canvasHeight = height - TOPBAR_HEIGHT - STATUSBAR_HEIGHT;
    glViewport(0, STATUSBAR_HEIGHT, canvasWidth, canvasHeight);
    glPolygonMode(GL_FRONT_AND_BACK, params.wireframe ? GL_LINE : GL_FILL);

    // Setup projection camera matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float ratio = (float) canvasWidth / canvasHeight;
    gluPerspective(45, ratio, 0.05, 1000);
    //glOrtho(-ratio, ratio, -1, 1, 0, 100);

    glMatrixMode(GL_MODELVIEW);

    mainShader.bind();
    mainShader.fillUniforms(params);
    mesh.bind();

    // Draw a cube
    glPushMatrix();
    {
        glTranslated(0, 0.06, 0);
        glScaled(meshSize, meshSize, meshSize);
        glTranslated(-0.5, 0, -0.5);

        mesh.draw();
    }
    glPopMatrix();

    mainShader.unbind();
    mesh.unbind();

    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    grid.bind();
    unliteShader.bind();
    grid.draw();
    grid.unbind();
    unliteShader.unbind();
    glPopAttrib();
}

void ApplicationRenderer::resize(int _width, int _height) {
    width = _width;
    height = _height;
}
