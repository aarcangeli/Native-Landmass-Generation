#include "Grid.h"

void Grid::generateGrid(uint32_t sizeX, uint32_t sizeZ, float dimension, Eigen::Vector3f color) {
    uint32_t lineCount = sizeX + sizeZ;
    uint32_t verticesCount = lineCount * 2;

    uint32_t baseVertex = (uint32_t) vertices.size();
    uint32_t baseFace = (uint32_t) faces.size();
    vertices.resize(baseVertex + verticesCount);
    faces.resize(baseFace + lineCount);

    float midDim = dimension / 2.f;
    uint32_t ii = baseVertex;
    uint32_t kk = baseFace;

    for (int i = 0; i < sizeX; i++) {
        float step = (i / (sizeX - 1.f) - .5f) * dimension;
        faces[kk++] = {ii, ii + 1};
        vertices[ii++].position = {step, 0, -midDim};
        vertices[ii++].position = {step, 0, +midDim};
    }

    for (int i = 0; i < sizeZ; i++) {
        float step = (i / (sizeX - 1.f) - .5f) * dimension;
        faces[kk++] = {ii, ii + 1};
        vertices[ii++].position = {-midDim, 0, step};
        vertices[ii++].position = {+midDim, 0, step};
    }

    ii = baseVertex;
    for (int i = 0; i < verticesCount; i++) {
        vertices[ii++].color = color;
    }
}

void Grid::generateAxes(float dimension) {
    uint32_t lineCount = 3;
    uint32_t verticesCount = lineCount * 2;

    uint32_t baseVertex = (uint32_t) vertices.size();
    uint32_t baseFace = (uint32_t) faces.size();
    vertices.resize(baseVertex + verticesCount);
    faces.resize(baseFace + lineCount);

    uint32_t ii = baseVertex;
    vertices[ii].color = {1, 0, 0};
    vertices[ii].position = {0, 0, 0};
    ii++;
    vertices[ii].color = {1, 0, 0};
    vertices[ii].position = {dimension, 0, 0};
    ii++;
    vertices[ii].color = {0, 1, 0};
    vertices[ii].position = {0, 0, 0};
    ii++;
    vertices[ii].color = {0, 1, 0};
    vertices[ii].position = {0, dimension, 0};
    ii++;
    vertices[ii].color = {0, 0, 1};
    vertices[ii].position = {0, 0, 0};
    ii++;
    vertices[ii].color = {0, 0, 1};
    vertices[ii].position = {0, 0, dimension};

    faces[baseFace] = {baseVertex, baseVertex + 1};
    faces[baseFace + 1] = {baseVertex + 2, baseVertex + 3};
    faces[baseFace + 2] = {baseVertex + 4, baseVertex + 5};
}
