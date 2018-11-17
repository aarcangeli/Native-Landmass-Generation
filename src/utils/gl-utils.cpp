#include "utils/gl-utils.h"

#include <SDL.h>
#include <GL/glew.h>
#include <iostream>


void GLAPIENTRY MessageCallback(GLenum source,
                                GLenum type,
                                GLuint id,
                                GLenum severity,
                                GLsizei length,
                                const GLchar *message,
                                const void *userParam) {
    const int GLUTILS_MAX_LOG_COUNT = 1000;
    static int logCount = 0;
    if (logCount == GLUTILS_MAX_LOG_COUNT) return;

    const char *typeDesc, *severityDesc;
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            typeDesc = "ERROR";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            typeDesc = "DEPRECATED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            typeDesc = "UNDEFINED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            typeDesc = "PORTABILITY";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            typeDesc = "PERFORMANCE";
            break;
        case GL_DEBUG_TYPE_OTHER:
            typeDesc = "OTHER";
            break;
        case GL_DEBUG_TYPE_MARKER:
            typeDesc = "MARKER";
            break;
        default:
            typeDesc = "LOG";
            break;
    }
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            severityDesc = "HIGH";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            severityDesc = "MEDIUM";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            severityDesc = "LOW";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            severityDesc = "NOTIFICATION";
            break;
        default:
            severityDesc = "LOG";
    }

    printf("OPENGL %s (%s): %s\n", typeDesc, severityDesc, message);

    logCount++;
    if (logCount == GLUTILS_MAX_LOG_COUNT) printf("OPENGL: Too many logs from OpenGL\n");
}

void installGlLogger() {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
}
