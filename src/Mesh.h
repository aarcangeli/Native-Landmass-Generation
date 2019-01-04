#ifndef MESH_H
#define MESH_H

#include "vector"
#include "GL/glew.h"
#include "utils/use-eigen.h"

struct Vertex {
    Eigen::Vector3f position;
    Eigen::Vector3f normal;
    Eigen::Vector2f textCoord;
    Eigen::Vector3f color;
    int avgFactor;
};

struct Line {
    static const int VERTICES_PER_FACE = 2;
    static const int MODE = GL_LINES;
    uint32_t vertex1;
    uint32_t vertex2;
};

struct Face {
    static const int VERTICES_PER_FACE = 4;
    static const int MODE = GL_QUADS;
    uint32_t vertex1;
    uint32_t vertex2;
    uint32_t vertex3;
    uint32_t vertex4;
};

/**
 * Generic mesh buffer
 * Template parameter 'FACE' must have the following fields:
 *   - static const int VERTICES_PER_FACE = 4;
 *     Number of vertices per each face
 *   - static const int MODE = GL_QUADS;
 *     Rendering mode
 */
template<class FACE>
class AbstractBuffer {
public:

    void init();
    void setPositionAttribute(GLint attribute);
    void setNormalAttribute(GLint attribute);
    void setTexCoordAttribute(GLint attribute);
    void setColorAttribute(GLint attribute);
    void refresh();
    void bind();
    void unbind();

    void resize(uint32_t numberOfVertices, uint32_t numberOfFaces);

    std::vector<Vertex> vertices;
    std::vector<FACE> faces;
    void draw();

protected:
    GLuint arrayBufferObject = 0, elementArrayBufferObject = 0;
    GLint positionAttribute = -1, normalAttribute = -1, texCoordAttribute = -1, colorAttribute = -1;
};

/**
 * A special kind of buffer with 4 vertices per face
 */
class Mesh : public AbstractBuffer<Face> {
public:
    void calculateNormals();
private:
    void applyNormal(uint64_t index, const Eigen::Vector3f &normal);
};

/**
 * A special kind of buffer with 2 vertices per face
 */
class LineBuffer : public AbstractBuffer<Line> {
};

#include "Mesh_impl.h"

#endif //MESH_H
