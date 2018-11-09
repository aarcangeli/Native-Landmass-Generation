#ifndef LANDMASSGENERATOR_H
#define LANDMASSGENERATOR_H

#include "vector"
#include "string"
#include "PerlinNoise.h"
#include "Mesh.h"
#include "vector"
#include "ChunkData.h"

// hardcoded in standard_fs.glsl
const uint32_t MAX_LAYERS = 8;

enum DrawMode {
    DRAW_MODE_NOISE,
    DRAW_MODE_COLOURS,
};

struct Color {
    float red, green, blue;
};

struct Texture {
    uint32_t width, height;
    std::vector<uint8_t> textureData;
    int myTex;
};

struct TerrainType {
    int textureNumber;
    Color colour;
    float startHeight;
    float blend;
    float colourStrength;
    float textureScale;
    int directGlTexture = -1; // fast getter

    TerrainType() = default;
    TerrainType(int textureNumber, const Color &colour, float startHeight, float blend, float colourStrength, float textureScale);
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

    std::vector<Texture> texturePalette;
    std::vector<TerrainType> layers;
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
};

static bool operator==(const Color &a, const Color &b) {
    return
            a.red == b.red &&
            a.green == b.green &&
            a.blue == b.blue;
}

static bool operator==(const TerrainType &a, const TerrainType &b) {
    return
            a.textureNumber == b.textureNumber &&
            a.colour == b.colour &&
            a.startHeight == b.startHeight &&
            a.blend == b.blend &&
            a.colourStrength == b.colourStrength &&
            a.textureScale == b.textureScale;
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
            a.offsetY == b.offsetY &&
            a.layers == b.layers;
}

static bool operator!=(LandmassParams &a, LandmassParams &b) {
    return !(a == b);
}

#endif //LANDMASSGENERATOR_H
