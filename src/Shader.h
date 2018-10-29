#ifndef NPCG_SHADER_H
#define NPCG_SHADER_H

class Shader {
    unsigned int vertexShader, fragmentShader, program;

public:

    bool compileVertexShader(const char *source);

    bool compileFragmentShader(const char *source);

    bool linkShader();

    bool compile(const char *vertexShaderSource, const char *fragmentShaderSource);

    void bind();

    void unbind();

    int getAttribLocation(const char *name);

private:

    bool checkCompilationStatus(const char *type, unsigned int shader);
};

#endif //NPCG_SHADER_H
