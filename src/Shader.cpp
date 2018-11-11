#include "Shader.h"
#include "stdio.h"
#include "iostream"
#include <algorithm>

#include <GL/glew.h>
#include "LandmassGenerator.h"

bool Shader::compileVertexShader(const char *source) {
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &source, nullptr);
    glCompileShader(vertexShader);

    return checkCompilationStatus("vertex shader", vertexShader);
}

bool Shader::compileFragmentShader(const char *source) {
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &source, nullptr);
    glCompileShader(fragmentShader);

    return checkCompilationStatus("fragment shader", fragmentShader);
}

bool Shader::linkShader() {
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint log_length, success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {

        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
        if (log_length > 0) {
            GLchar *log = (GLchar *) malloc(log_length);
            glGetProgramInfoLog(program, log_length, nullptr, log);
            printf("shader link log: %s\n", log);
            free(log);
        }

        printf("shader link error");
        exit(EXIT_FAILURE);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

bool Shader::checkCompilationStatus(const char *type, GLuint shader) {
    GLint log_length, success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        printf("ERROR: Cannot compile %s:\n", type);
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
        if (log_length > 0) {
            GLchar *log = (GLchar *) malloc(log_length);
            glGetShaderInfoLog(shader, log_length, nullptr, log);
            printf("%s\n", log);
            free(log);
        }
        return false;
    }

    return true;
}

void Shader::bind() {
    glUseProgram(program);
}

void Shader::unbind() {
    glUseProgram(0);
}

GLint Shader::getAttribLocation(const char *name) {
    return glGetAttribLocation(program, name);
}

GLint Shader::getUniformLocation(const char *name) {
    return glGetUniformLocation(program, name);
}

bool Shader::compile(const char *vertexShaderSource, const char *fragmentShaderSource) {
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
