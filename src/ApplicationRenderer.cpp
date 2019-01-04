#include "ApplicationRenderer.h"
#include "Application.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL.h>
#include <string>

#include "res_glsl.h"
#include "Gui.h"

ApplicationRenderer::ApplicationRenderer() = default;

void ApplicationRenderer::init(Application *_app) {
    app = _app;
    ResourceLoader *loader = app->getLoader();

    // speedup shader creation
#define LOAD_SHADER(name, watch) loader->loadShader(#name, glsl::name##_vs, glsl::name##_fs, watch)

    standardShader = LOAD_SHADER(standard, true);
    unliteShader = LOAD_SHADER(unlite, true);

#undef LOAD_SHADER

    mesh.init();
    mesh.setPositionAttribute(LAYOUT::POSITION);
    mesh.setNormalAttribute(LAYOUT::NORMAL);
    mesh.setTexCoordAttribute(LAYOUT::UV);

    grid.init();
    grid.generateAxes(5);
    grid.generateGrid(11, 11, 10, {.4f, .4f, .4f});
    grid.bind();
    grid.refresh();
    grid.setPositionAttribute(LAYOUT::POSITION);
    grid.setColorAttribute(LAYOUT::COLOR);
}

void ApplicationRenderer::updateMesh(const LandmassParams &params, const ChunkData &chunk) {
    bool useFastNormals = params.realtime != 0;
    chunk.updateMesh(mesh, useFastNormals);

    // update mesh
    mesh.bind();
    mesh.refresh();
}

void ApplicationRenderer::render(const LandmassParams &params) {
    const float meshSize = 2;
    glm::mat4 viewMat = app->getViewMatrix();

    // Setup OpenGL
    glEnable(GL_DEPTH_TEST);
    int canvasWidth = width - SIDEBAR_WIDTH;
    int canvasHeight = height - TOPBAR_HEIGHT - STATUSBAR_HEIGHT;
    glViewport(0, STATUSBAR_HEIGHT, canvasWidth, canvasHeight);
    glPolygonMode(GL_FRONT_AND_BACK, app->wireframe ? GL_LINE : GL_FILL);

    // Setup projection camera matrix
    float ratio = (float) canvasWidth / canvasHeight;
    glm::mat4 projMat = glm::perspective<float>(45, ratio, 0.05f, 1000);
    //glm::mat4 projMat = glm::ortho<float>(-ratio, ratio, -1, 1, 0, 100);

    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::scale(modelMat, glm::vec3(meshSize));
    modelMat = glm::translate(modelMat, glm::vec3(-0.5, 0, -0.5));

    standardShader->bind(modelMat, viewMat, projMat);
    standardShader->fillUniforms(params);
    mesh.bind();

    mesh.draw();

    standardShader->unbind();
    mesh.unbind();

    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    grid.bind();
    unliteShader->bind(glm::mat4(1.0f), viewMat, projMat);
    grid.draw();
    grid.unbind();
    unliteShader->unbind();
    glPopAttrib();
}

void ApplicationRenderer::resize(int _width, int _height) {
    width = _width;
    height = _height;
}
