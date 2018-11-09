#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H

#include <GL/glew.h>
#include "LandmassGenerator.h"

class ResourceLoader {
public:
    bool init();

    Texture loadTexture(LandmassParams &params,
                        const char *name,
                        const char *start, const char *end,
                        GLint minFilter = GL_LINEAR,
                        GLint magFilter = GL_LINEAR);

};


#endif //RESOURCELOADER_H
