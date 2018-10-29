#include "Shader.h"
#include "stdio.h"
#include "iostream"

#include <GL/glew.h>

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

bool Shader::compile(const char *vertexShaderSource, const char *fragmentShaderSource) {
    if (!compileVertexShader(vertexShaderSource)) return false;
    if (!compileFragmentShader(fragmentShaderSource)) return false;
    return linkShader();
}
