#include "ChunkData.h"
#include <algorithm>

using namespace std;

ChunkData::ChunkData() { resize(0, 0); };

void ChunkData::resize(uint32_t _width, uint32_t _height) {
    assert(_width >= 0);
    assert(_height >= 0);

    if (_width == width && _height == height)
        return;

    width = _width;
    height = _height;
    vertexCount = width * height;

    heightmap.resize(vertexCount);
    textureData.resize(vertexCount * 4);
}

uint32_t ChunkData::getWidth() const {
    return width;
}

uint32_t ChunkData::getHeight() const {
    return height;
}

void ChunkData::updateMesh(Mesh &mesh, bool fastNormal) {
    // each vertex has a face apart the last row and the last column
    uint32_t numberOfFaces = vertexCount - width - height + 1;

    mesh.resize(vertexCount, numberOfFaces);

    // fill the vertices
    uint32_t ii = 0;
    auto &vertices = mesh.vertices;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            auto &v = vertices[ii];
            v.position = {(float) x / width, heightmap[ii], (float) y / height};
            v.textCoord[0] = (float) x / width;
            v.textCoord[1] = (float) y / height;
            ii++;
        }
    }

    // fill the indexes
    auto *face = &mesh.faces.front();
    for (uint32_t y = 1; y < height; y++) {
        for (uint32_t x = 1; x < width; x++) {
            face->vertex1 = (x - 1) + (y - 1) * width;
            face->vertex2 = (x - 1) + y * width;
            face->vertex3 = x + y * width;
            face->vertex4 = x + (y - 1) * width;
            face++;
        }
    }

    if (fastNormal) {
        // fast normal calculation
        ii = 0;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                auto &v1 = vertices[ii];
                if (x == 0 || y == 0) {
                    // the corners will have a normal that point to the sky
                    v1.normal = {0, 1, 0};
                } else {
                    auto &u = vertices[ii - width].position - v1.position;
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
    } else {
        // slow method
        mesh.calculateNormals();
    }
}
