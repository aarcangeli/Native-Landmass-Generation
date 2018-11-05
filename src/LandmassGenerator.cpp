#include "LandmassGenerator.h"
#include "algorithm"

void LandmassGenerator::configure(const LandmassParams &_params) {
    params = _params;
}

void LandmassGenerator::generateChunk(ChunkData &buffer, uint32_t bufferWidth, uint32_t bufferHeight, const Region &region) {
    buffer.resize(bufferWidth, bufferHeight);
    generateHeightmap(params, region, buffer.heightmap.data(), bufferWidth, bufferHeight);

    switch (params.mode) {
        case DRAW_MODE_NOISE:
            drawHeightMapTexture(buffer.heightmap.data(), buffer.textureData.data(), bufferWidth, bufferHeight);
            break;
        case DRAW_MODE_COLOURS:
            drawColorTexture(buffer.heightmap.data(), buffer.textureData.data(), bufferWidth, bufferHeight);
            break;
    }
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

void LandmassGenerator::drawHeightMapTexture(float *heightMap, float *textureData, int width, int height) {
    int totalSize = width * height;
    float min = *std::min_element(heightMap, heightMap + totalSize);
    float max = *std::max_element(heightMap, heightMap + totalSize);

    unsigned int ii = 0;
    unsigned int kk = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            float noiseHeight = heightMap[ii++];

            // apply level transform
            noiseHeight = (noiseHeight + min) / (max - min);

            // calculate noise
            textureData[kk + 0] = noiseHeight;
            textureData[kk + 1] = noiseHeight;
            textureData[kk + 2] = noiseHeight;
            textureData[kk + 3] = 1;
            kk += 4;
        }
    }
}

void LandmassGenerator::drawColorTexture(float *heightMap, float *textureData, int width, int height) {
    unsigned int ii = 0;
    unsigned int kk = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            float noiseHeight = heightMap[ii++];
            textureData[kk + 0] = 1;
            textureData[kk + 1] = 0;
            textureData[kk + 2] = 0;
            textureData[kk + 3] = 1;
            kk += 4;
        }
    }
}
