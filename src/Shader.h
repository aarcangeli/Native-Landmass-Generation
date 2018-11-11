#ifndef NPCG_SHADER_H
#define NPCG_SHADER_H

#include "LandmassGenerator.h"

class Shader {
    unsigned int vertexShader, fragmentShader, program;

public:

    bool compile(const char *vertexShaderSource, const char *fragmentShaderSource);

    void bind();

    void unbind();

    int getAttribLocation(const char *name);

    void fillUniforms(const LandmassParams &params);
    GLint getUniformLocation(const char *name);

private:
    GLint heightRangeLocation;
    GLint layerTextLocation;
    GLint layerCountLocation;
    GLint layerPackListLocation;

    bool checkCompilationStatus(const char *type, unsigned int shader);
    bool compileVertexShader(const char *source);
    bool compileFragmentShader(const char *source);
    bool linkShader();
};

#endif //NPCG_SHADER_H
