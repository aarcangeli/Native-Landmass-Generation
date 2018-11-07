#include "ResourceLoader.h"
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

GLuint ResourceLoader::loadTextureFromRes(const char *name, const char *start, const char *end, GLint minFilter, GLint magFilter) {
    GLuint ret;
    glGenTextures(1, &ret);
    glBindTexture(GL_TEXTURE_2D, ret);
    SDL_Surface *rawData = IMG_Load_RW(SDL_RWFromMem((void *) start, (int) (end - start)), 1);
    if (!rawData) {
        printf("ERROR: Cannot load %s\n", name);
        return 0;
    }
    SDL_Surface *grass = SDL_CreateRGBSurfaceWithFormat(0, rawData->w, rawData->h, 8, SDL_PIXELFORMAT_RGBA32);
    if (SDL_BlitSurface(rawData, nullptr, grass, nullptr) < 0) {
        printf("ERROR: Cannot load %s: %s\n", name, SDL_GetError());
        return 0;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, grass->w, grass->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, grass->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    return ret;
}

bool ResourceLoader::init() {
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Cannot load SDL_image: %s\n", IMG_GetError());
        return false;
    }
    return true;
}
