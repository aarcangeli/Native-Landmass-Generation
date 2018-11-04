#include "landmass.h"
#include "math.h"

void generateNoiseMap(float *noiseData, PerlinNoise &pn, int size, LandmassParams &params) {
    float scale = params.scale;
    float z = params.z;
    unsigned int ii = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            float x = (j - size / 2.f) / size / scale;
            float y = (i - size / 2.f) / size / scale;

            float amplitude = 1;
            float frequency = 1;
            float noiseHeight = 0;

            for (int k = 0; k < params.octaves; k++) {
                float mapX = x * frequency + params.offsetX;
                float mapY = y * frequency + params.offsetY;
                // n is in range [-1,1]
                float n = (float) pn.noise(mapX, mapY, z) * 2 - 1;
                noiseHeight += n * amplitude;

                // advance
                amplitude *= params.persistence;
                frequency *= params.lacunarity;
            }

            // calculate noise
            noiseData[ii++] = noiseHeight;
        }
    }
}

void convertNoiseMapToTexture(float *textureData, float *noiseData, int size, LandmassParams &params) {
    // get level range
    float level = 0;
    float amplitude = 1;
    for (int i = 0; i < params.octaves; ++i) {
        level += amplitude;
        amplitude *= params.persistence;
    }
    level = level * 0.7f;

    unsigned int ii = 0;
    unsigned int kk = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            float noiseHeight = noiseData[ii++];

            // apply level transform
            noiseHeight = (noiseHeight + level) / (level * 2);

            // calculate noise
            if (params.mode == DRAW_MODE_COLOURS) {
                // get color
                textureData[kk + 0] = 0;
                textureData[kk + 1] = 0;
                textureData[kk + 2] = 0;
            } else {
                textureData[kk + 0] = noiseHeight;
                textureData[kk + 1] = noiseHeight;
                textureData[kk + 2] = noiseHeight;
            }
            kk += 3;
        }
    }
}

void updateMesh(Mesh &mesh, const float *noiseData, int size, LandmassParams &params) {
    // every cell of the matrix is mappet into a vertex
    // so the number of vertices is the same as the number of items in noiseData
    uint32_t numberOfVertices = (uint32_t) (size * size);

    // each face has a vertex apart the last row and the last column
    uint32_t numberOfFaces = numberOfVertices - size - size + 1;

    mesh.resize(numberOfVertices, numberOfFaces);

    // fill the vertices
    uint32_t ii = 0;
    auto &vertices = mesh.vertices;
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            auto &v = vertices[ii];
            v.position = {(float) x / size, noiseData[ii], (float) y / size};
            v.textCoord[0] = (float) x / size;
            v.textCoord[1] = (float) y / size;

            glTexCoord2f((float) x / size, (float) y / size);

            ii++;
        }
    }

    // fast normal calculation
    if (params.realtime) {
        ii = 0;
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                auto &v1 = vertices[ii];
                if (x == 0 || y == 0) {
                    v1.normal = {0, 1, 0};
                } else {
                    auto &u = vertices[ii - size].position - v1.position;
                    auto &v = vertices[ii - 1].position - v1.position;
                    v1.normal = {
                            (u[1] * v[2]) - (u[2] * v[1]),
                            (u[2] * v[0]) - (u[0] * v[2]),
                            (u[0] * v[1]) - (u[1] * v[0]),
                    };
                }
                ii++;
            }
        }
    }

    // fill the indexes
    auto *face = &mesh.faces.front();
    for (uint32_t y = 1; y < size; y++) {
        for (uint32_t x = 1; x < size; x++) {
            face->vertex1 = (x - 1) + (y - 1) * size;
            face->vertex2 = (x - 1) + y * size;
            face->vertex3 = x + y * size;
            face->vertex4 = x + (y - 1) * size;
            face++;
        }
    }

    // this method is too expensive for real time
    if (!params.realtime) {
        mesh.calculateNormals();
    }
    mesh.bind();
    mesh.refresh();
}
