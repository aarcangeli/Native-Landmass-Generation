#ifndef NLG_H
#define NLG_H

#include "vector"
#include "string"

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

struct NoiseParams {
    DrawMode mode = DRAW_MODE_NOISE;
    int realtime = 0;
    int refreshRequested = 1;

    float scale = 0.3;

    float z = 0;
    int octaves = 3;
    float persistence = 0.5;
    float lacunarity = 2;
    float offsetX = 0;
    float offsetY = 0;

    std::vector<TerrainType> types;
};

#endif //NLG_H
