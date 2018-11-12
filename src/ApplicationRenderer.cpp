#include "ApplicationRenderer.h"

#include <SDL.h>
#include <string>

#include "res_glsl.h"
#include "Gui.h"

ApplicationRenderer::ApplicationRenderer() {
}

void ApplicationRenderer::init() {
    if (!mainShader.compile(glsl::standard_vs, glsl::standard_fs)) {
        printf("ERROR: Cannot compile standard shader\n");
        throw;
    }

    mesh.init();
    mesh.setPositionAttribute(mainShader.getAttribLocation("position"));
    mesh.setNormalAttribute(mainShader.getAttribLocation("normal"));
    mesh.setTexCoordAttribute(mainShader.getAttribLocation("uv"));
}

void ApplicationRenderer::updateMesh(const LandmassParams &params, const ChunkData &chunk) {
    bool useFastNormals = params.realtime != 0;
    chunk.updateMesh(mesh, useFastNormals);

    // update mesh
    mesh.bind();
    mesh.refresh();
}

void ApplicationRenderer::render(const LandmassParams &params) {
    const double meshSize = 3;

    // Setup OpenGL
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width - SIDEBAR_WIDTH, height);
    glPolygonMode(GL_FRONT_AND_BACK, params.wireframe ? GL_LINE : GL_FILL);

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

void ApplicationRenderer::resize(int _width, int _height) {
    width = _width;
    height = _height;
}
