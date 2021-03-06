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
    void updateMesh(Mesh &mesh, bool fastNormal) const;

    void drawHeightMapTexture(std::vector<float> &textureData);

    inline uint32_t getWidth() const { return width; }
    inline uint32_t getHeight() const { return height; }

    inline std::vector<float> &getHeightMap() { return heightMap; };

private:
    uint32_t width, height;
    uint32_t vertexCount;
    std::vector<float> heightMap;
};

#endif //CHUNKDATA_H
