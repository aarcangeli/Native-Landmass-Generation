#include "ResourceLoader.h"
#include "LandmassGenerator.h"
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

bool ResourceLoader::init() {
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Cannot load SDL_image: %s\n", IMG_GetError());
        return false;
    }
    return true;
}

Texture ResourceLoader::loadTexture(LandmassParams &params, const char *name, const char *start, const char *end, GLint minFilter,
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
    result.textureData.resize(result.width * result.height * 3);
    result.myTex = tex;
    memcpy(result.textureData.data(), converted->pixels, result.textureData.size());
    SDL_FreeSurface(converted);
    return result;
}
