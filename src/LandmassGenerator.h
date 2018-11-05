#ifndef LANDMASSGENERATOR_H
#define LANDMASSGENERATOR_H

#include "vector"
#include "string"
#include "PerlinNoise.h"
#include "Mesh.h"
#include "vector"
#include "ChunkData.h"

enum DrawMode {
    DRAW_MODE_NOISE,
    DRAW_MODE_COLOURS,
};

struct TerrainType {
    GLuint texture;
};

//! It determinate the region of landmass to be generated
struct Region {
    float positionX, positionY;
    float width, height;
};

struct LandmassParams {
    DrawMode mode = DRAW_MODE_COLOURS;
    int realtime = 0;
    int refreshRequested = 1;

    float scale = 0.25;
    float heightMultiplier = 0.06;

    float z = 0;
    int octaves = 3;
    float persistence = 0.5;
    float lacunarity = 2;
    float offsetX = 0;
    float offsetY = 0;

    TerrainType water, sand, grass, rock, snow;
};


class LandmassGenerator {
public:

    void configure(const LandmassParams &params);

    /*!
     * Generate a chunk of data
     *
     * @param buffer
     * @param bufferWidth the size of data to generate
     * @param bufferHeight
     * @param region the region
     */
    void generateChunk(ChunkData &buffer, uint32_t bufferWidth, uint32_t bufferHeight, const Region &region);

private:
    PerlinNoise pn;
    LandmassParams params;

    void generateHeightmap(LandmassParams &params, const Region &region, float *heightMap, int width, int height);

    void drawHeightMapTexture(float *heightMap, float *textureData, int width, int height);
    void drawColorTexture(float *heightMap, float *textureData, int width, int height);
};

static bool operator==(const TerrainType &a, const TerrainType &b) {
    return
            a.texture == b.texture;
}

static bool operator==(const LandmassParams &a, const LandmassParams &b) {
    return
            a.mode == b.mode &&
            a.realtime == b.realtime &&
            a.scale == b.scale &&
            a.heightMultiplier == b.heightMultiplier &&
            a.z == b.z &&
            a.octaves == b.octaves &&
            a.persistence == b.persistence &&
            a.lacunarity == b.lacunarity &&
            a.offsetX == b.offsetX &&
            a.water == b.water &&
            a.sand == b.sand &&
            a.grass == b.grass &&
            a.rock == b.rock &&
            a.snow == b.snow;
}

static bool operator!=(LandmassParams &a, LandmassParams &b) {
    return !(a == b);
}

#endif //LANDMASSGENERATOR_H
