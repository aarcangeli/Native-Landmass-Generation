#ifndef MESH_H
#define MESH_H

#include "vector"
#include "GL/glew.h"

/**
 * General purpose mesh buffer
 * Each vertex has the following structure:
 *   - float[3] position
 *   - float[3] normal
 *   - float[2] uv coordinates
 *
 * In this implementation every face has 4 vertices
 */
class Mesh {

public:

    struct Vertex {
        // position
        float x, y, z;
        // normal
        float nx, ny, nz;
        // Texture Coordinates
        float u, v;
    };

    union Face {
        struct {
            uint32_t vertex1;
            uint32_t vertex2;
            uint32_t vertex3;
            uint32_t vertex4;
        };
        uint32_t index;
    };

    bool init();
    void setPositionAttribute(GLint attribute);
    void setNormalAttribute(GLint attribute);
    void setTexCoordAttribute(GLint attribute);
    void refresh();
    void bind();
    void unbind();

    void resize(uint32_t numberOfVertices, uint32_t numberOfFaces);

    std::vector<Vertex> verticies;
    std::vector<Face> faces;
    void draw();
private:

    GLuint arrayBufferObject, elementArrayBufferObject;
    GLint positionAttribute = -1, normalAttribute = -1, texCoordAttribute = -1;
};


#endif //MESH_H
