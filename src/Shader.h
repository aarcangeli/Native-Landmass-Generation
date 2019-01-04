#ifndef NLG_SHADER_H
#define NLG_SHADER_H

class Application;

#include "LandmassGenerator.h"
#include "glm/glm.hpp"

enum LAYOUT {
    POSITION = 0,
    NORMAL = 1,
    COLOR = 2,
    UV = 3,
};


class Shader {
    uint32_t vertexShader = 0;
    uint32_t fragmentShader = 0;
    uint32_t program = 0;

public:

    void init();
    bool compile(const char *vertexShaderSource, const char *fragmentShaderSource);

    void bind(const glm::mat4 &modelMat, const glm::mat4 &viewMat, const glm::mat4 &projMat);
    void unbind();

    void fillUniforms(const LandmassParams &params);
    GLint getUniformLocation(const char *name);

private:
    bool isInitialized = false;
    GLint heightRangeLocation;
    GLint layerTextLocation;
    GLint layerCountLocation;
    GLint layerPackListLocation;

    GLint modelMatIdx;
    GLint viewMatIdx;
    GLint projMatIdx;

    bool checkCompilationStatus(const char *type, unsigned int shader);
    bool compileVertexShader(const char *source);
    bool compileFragmentShader(const char *source);
    bool linkShader();
};

#endif //NLG_SHADER_H
