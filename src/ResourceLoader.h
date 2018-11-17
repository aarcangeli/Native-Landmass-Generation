#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H

#include <memory>
#include <GL/glew.h>
#include "LandmassGenerator.h"
#include "Shader.h"

class ResourceLoader {
public:
    void init();

    Texture loadTexture(const char *name,
                        const char *start, const char *end,
                        GLint minFilter = GL_LINEAR,
                        GLint magFilter = GL_LINEAR);


    std::shared_ptr<Shader> loadShader(const char *name,
                                       const char *shaderVS,
                                       const char *shaderFS,
                                       bool watch);

    void digest();

private:
    struct ShaderFile {
        std::weak_ptr<Shader> shader;
        std::string vsFilename;
        std::string fsFilename;
        time_t vsModificationTime;
        time_t fsModificationTime;
    };

    std::vector<ShaderFile> watches;

    std::shared_ptr<Shader> loadFromFileSystem(const char *name, bool watch);
    bool compileShaderFile(ShaderFile &shaderFile) const;
    void compileFallbackShader(std::shared_ptr<Shader> &result) const;
};


#endif //RESOURCELOADER_H
