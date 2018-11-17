#include "config.h"
#include "ResourceLoader.h"
#include "LandmassGenerator.h"
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <fstream>
#include "res_glsl.h"

// for stat()
#include <sys/stat.h>

#ifndef WIN32
#include <unistd.h>
#define stat _stat
#endif

void ResourceLoader::init() {
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Cannot load SDL_image: %s\n", IMG_GetError());
        throw;
    }
}

Texture ResourceLoader::loadTexture(const char *name, const char *start, const char *end, GLint minFilter,
                                    GLint magFilter) {
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    SDL_Surface *rawData = IMG_Load_RW(SDL_RWFromMem((void *) start, (int) (end - start)), 1);
    if (!rawData) {
        printf("ERROR: Cannot load %s\n", name);
        throw;
    }
    SDL_Surface *converted = SDL_CreateRGBSurfaceWithFormat(0, rawData->w, rawData->h, 8, SDL_PIXELFORMAT_RGBA32);
    if (SDL_BlitSurface(rawData, nullptr, converted, nullptr) < 0) {
        printf("ERROR: Cannot load %s: %s\n", name, SDL_GetError());
        throw;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, converted->w, converted->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, converted->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    Texture result{};
    result.width = static_cast<uint32_t>(rawData->w);
    result.height = static_cast<uint32_t>(rawData->h);
    result.textureData.resize(result.width * result.height);
    result.myTex = tex;
    memcpy(result.textureData.data(), converted->pixels, result.width * result.height * 4);
    SDL_FreeSurface(converted);
    return result;
}

std::shared_ptr<Shader> ResourceLoader::loadShader(const char *name, const char *shaderVS, const char *shaderFS, bool watch) {
#if !NLG_RELEASE
    // try to load from file system
    if (name != std::string("blank")) {
        std::shared_ptr<Shader> fromFs = loadFromFileSystem(name, watch);
        if (fromFs) return fromFs;
    }
#endif

    std::shared_ptr<Shader> result{new Shader};
    if (!result->compile(shaderVS, shaderFS)) {
        printf("ERROR: Cannot compile shader '%s'\n", name);
        compileFallbackShader(result);
    }

    return result;
}

std::shared_ptr<Shader> ResourceLoader::loadFromFileSystem(const char *name, bool watch) {
    std::__cxx11::string path = __FILE__;
    replace(path.begin(), path.end(), '\\', '/');
    int64_t index = path.find_last_of('/');
    if (index < 0) return nullptr;
    path = path.substr(0, index) + "/shaders/" + name;

    std::shared_ptr<Shader> result{new Shader};
    ShaderFile shaderFile = {
            result,
            path + +"_vs.glsl",
            path + +"_fs.glsl",
            0, 0
    };

    if (!compileShaderFile(shaderFile)) return nullptr;

    if (watch) {
        // set last modification time
        struct stat fileStat{};
        if (stat(shaderFile.vsFilename.c_str(), &fileStat) == 0) {
            shaderFile.vsModificationTime = fileStat.st_mtime;
        }
        if (stat(shaderFile.fsFilename.c_str(), &fileStat) == 0) {
            shaderFile.fsModificationTime = fileStat.st_mtime;
        }
        watches.push_back(shaderFile);
    }

    return result;
}

bool ResourceLoader::compileShaderFile(ShaderFile &shaderFile) const {
    std::ifstream file;
    std::string vs, fs;

    std::shared_ptr<Shader> result = shaderFile.shader.lock();
    assert(result);

    // read vertex shader
    file.open(shaderFile.vsFilename);
    if (!file.is_open()) return false;
    file.seekg(0, std::ios_base::end);
    vs.reserve(file.tellg());
    file.seekg(0, std::ios_base::beg);

    vs.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    printf("'%s' loaded\n", shaderFile.vsFilename.c_str());

    // read fragment shader
    file.open(shaderFile.fsFilename);
    if (!file.is_open()) return false;
    file.seekg(0, std::ios_base::end);
    fs.reserve(file.tellg());
    file.seekg(0, std::ios_base::beg);

    fs.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    printf("'%s' loaded\n", shaderFile.fsFilename.c_str());

    if (!result->compile(vs.c_str(), fs.c_str())) {
        compileFallbackShader(result);
    }

    return true;
}

void ResourceLoader::digest() {
    struct stat fileStat{};
    for (auto &shaderFile : watches) {
        bool needUpdated = false;
        if (stat(shaderFile.vsFilename.c_str(), &fileStat) == 0) {
            if (shaderFile.vsModificationTime != fileStat.st_mtime) {
                shaderFile.vsModificationTime = fileStat.st_mtime;
                needUpdated = true;
            }
        }
        if (stat(shaderFile.fsFilename.c_str(), &fileStat) == 0) {
            if (shaderFile.fsModificationTime != fileStat.st_mtime) {
                shaderFile.fsModificationTime = fileStat.st_mtime;
                needUpdated = true;
            }
        }
        if (needUpdated) {
            std::shared_ptr<Shader> shader = shaderFile.shader.lock();
            if (shader && !compileShaderFile(shaderFile)) {
                compileFallbackShader(shader);
            }
        }
    }
}

void ResourceLoader::compileFallbackShader(std::shared_ptr<Shader> &result) const {
    if (!result->compile(glsl::fallback_vs, glsl::fallback_fs)) {
        printf("FATAL_ERROR: Cannot compile shader 'blank'\n");
    }
}
