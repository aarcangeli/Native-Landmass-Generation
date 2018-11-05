#ifndef CHUNKDATA_H
#define CHUNKDATA_H

#include "Mesh.h"
#include "vector"

class ChunkData {
public:
    /// Initialize an empty chunk of landmass
    ChunkData();

    /*!
     * Resize the chunk to a specified size in faces
     * The number of vertices can be calculated with the formula (width+1) * (height+1)
     *
     * @param width number of vertices in width
     * @param height number of vertices in height
     */
    void resize(uint32_t width, uint32_t height);

    /*!
     * Generate a mesh from this chunk of landmass
     *
     * @param mesh
     * @param fastNormal
     */
    void updateMesh(Mesh &mesh, bool fastNormal);

    void drawHeightMapTexture(std::vector<float> &textureData);
    void drawColorTexture(std::vector<float> &textureData);

    inline uint32_t getWidth() const;
    inline uint32_t getHeight() const;

    std::vector<float> heightMap;

private:
    uint32_t width, height;
    uint32_t vertexCount;
};

#endif //CHUNKDATA_H
