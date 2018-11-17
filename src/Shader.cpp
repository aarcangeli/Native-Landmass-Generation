#include "Shader.h"
#include "stdio.h"
#include "iostream"
#include <algorithm>
#include <Application.h>

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include "LandmassGenerator.h"

void Shader::init() {
    if (isInitialized) return;
    program = glCreateProgram();
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    if (!program || !vertexShader || !fragmentShader) {
        printf("Cannot create shader\n");
        throw;
    }
}

bool Shader::compileVertexShader(const char *source) {
    glShaderSource(vertexShader, 1, &source, nullptr);
    glCompileShader(vertexShader);
    return checkCompilationStatus("vertex shader", vertexShader);
}

bool Shader::compileFragmentShader(const char *source) {
    glShaderSource(fragmentShader, 1, &source, nullptr);
    glCompileShader(fragmentShader);
    return checkCompilationStatus("fragment shader", fragmentShader);
}

bool Shader::linkShader() {
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) return false;

    modelMatIdx = glGetUniformLocation(program, "modelMat");
    viewMatIdx = glGetUniformLocation(program, "viewMat");
    projMatIdx = glGetUniformLocation(program, "projMat");

    return true;
}

bool Shader::checkCompilationStatus(const char *type, GLuint shader) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    return success != 0;
}

void Shader::bind(const glm::mat4 &modelMat, const glm::mat4 &viewMat, const glm::mat4 &projMat) {
    glUseProgram(program);

    glUniformMatrix4fv(modelMatIdx, 1, GL_FALSE, glm::value_ptr(modelMat));
    glUniformMatrix4fv(viewMatIdx, 1, GL_FALSE, glm::value_ptr(viewMat));
    glUniformMatrix4fv(projMatIdx, 1, GL_FALSE, glm::value_ptr(projMat));
}

void Shader::unbind() {
    glUseProgram(0);
}

GLint Shader::getUniformLocation(const char *name) {
    return glGetUniformLocation(program, name);
}

bool Shader::compile(const char *vertexShaderSource, const char *fragmentShaderSource) {
    init();
    if (!compileVertexShader(vertexShaderSource)) return false;
    if (!compileFragmentShader(fragmentShaderSource)) return false;
    if (!linkShader()) return false;
    layerTextLocation = getUniformLocation("layerText");
    heightRangeLocation = getUniformLocation("heightRange");
    layerCountLocation = getUniformLocation("layerCount");
    layerPackListLocation = getUniformLocation("layerPackList");
    return true;
}

void Shader::fillUniforms(const LandmassParams &params) {
    int layerText[MAX_LAYERS];
    float layerPackList[MAX_LAYERS * 4];

    uint32_t layerCount = std::min((uint32_t) params.layers.size(), MAX_LAYERS);

    for (int i = 0, ii = 0, jj = 0; i < layerCount; ++i) {
        const TerrainType &terrain = params.layers[i];
        layerPackList[jj++] = terrain.height;
        layerPackList[jj++] = terrain.blend;
        layerPackList[jj++] = terrain.textureScale;

        layerText[i] = i;

        glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + i));
        glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(terrain.directGlTexture));
    }

    glUniform1i(layerCountLocation, layerCount);
    glUniform2f(heightRangeLocation, 0.0, 1.0);
    glUniform3fv(layerPackListLocation, layerCount, layerPackList);
    glUniform1iv(layerTextLocation, layerCount, layerText);
}
