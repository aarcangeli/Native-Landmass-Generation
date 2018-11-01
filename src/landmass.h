#ifndef LANDMASSGENERATOR_H
#define LANDMASSGENERATOR_H

#include "vector"
#include "string"
#include "PerlinNoise.h"
#include "Mesh.h"

enum DrawMode {
    DRAW_MODE_NOISE,
    DRAW_MODE_COLOURS,
};

struct Color {
    float red, green, blue;
};

struct TerrainType {
    std::string name;
    float height;
    Color color;
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

    std::vector<TerrainType> types;
};

static bool operator==(const Color &a, const Color &b) {
    return
            a.red == b.red &&
            a.green == b.green &&
            a.blue == b.blue;
}

static bool operator==(const TerrainType &a, const TerrainType &b) {
    return
            a.name == b.name &&
            a.height == b.height &&
            a.color == b.color;
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
            a.types == b.types;
}

static bool operator!=(LandmassParams &a, LandmassParams &b) {
    return !(a == b);
}

void generateNoiseMap(float *noiseData, PerlinNoise &pn, int size, LandmassParams &params);
void convertNoiseMapToTexture(float *textureData, float *noiseData, int size, LandmassParams &params);
void updateMesh(Mesh &mesh, const float *noiseData, int size, LandmassParams &params);

#endif //LANDMASSGENERATOR_H
