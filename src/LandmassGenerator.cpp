#include "LandmassGenerator.h"
#include "algorithm"

TerrainType::TerrainType(int textureNumber, const Color &colour, float startHeight, float blend, float colourStrength, float textureScale)
        : textureNumber(textureNumber), colour(colour), startHeight(startHeight), blend(blend), colourStrength(colourStrength),
          textureScale(textureScale) {}

void LandmassGenerator::configure(const LandmassParams &_params) {
    params = _params;
}

void LandmassGenerator::generateChunk(ChunkData &buffer, uint32_t bufferWidth, uint32_t bufferHeight, const Region &region) {
    buffer.resize(bufferWidth, bufferHeight);
    generateHeightmap(params, region, buffer.heightMap.data(), bufferWidth, bufferHeight);
}

void LandmassGenerator::generateHeightmap(LandmassParams &params, const Region &region, float *heightMap, int width, int height) {
    unsigned int ii = 0;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // map buffer coordinates into global landmass coordinates
            float x = region.positionX + (float) j / width / region.width;
            float y = region.positionY + (float) i / height / region.height;

            float amplitude = 1;
            float frequency = 1;
            float noiseHeight = 0;

            for (int k = 0; k < params.octaves; k++) {
                float mapX = x * frequency + params.offsetX;
                float mapY = y * frequency + params.offsetY;
                double noise = pn.noise(mapX, mapY, params.z);
                // n is in now in range [-1,1]
                float n = (float) noise * 2 - 1;
                noiseHeight += n * amplitude;

                // advance
                amplitude *= params.persistence;
                frequency *= params.lacunarity;
            }

            noiseHeight = (float) pow(10, noiseHeight);
            noiseHeight = noiseHeight * params.heightMultiplier;

            // calculate noise
            heightMap[ii++] = noiseHeight;
        }
    }
}
