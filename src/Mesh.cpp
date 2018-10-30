#include <cassert>
#include "Mesh.h"

bool Mesh::init() {
    glGenBuffers(1, &arrayBufferObject);
    glGenBuffers(1, &elementArrayBufferObject);
    return true;
}

void Mesh::refresh() {
#ifndef NDEBUG
    // check that the buffer are currently binded
    GLint value;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &value);
    assert(value == arrayBufferObject);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &value);
    assert(value == elementArrayBufferObject);
#endif

    // build semantic
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(Face), faces.data(), GL_DYNAMIC_DRAW);
}

void Mesh::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, arrayBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBufferObject);
    // build semantic
    if (positionAttribute >= 0) {
        glVertexAttribPointer((GLuint) positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glEnableVertexAttribArray((GLuint) positionAttribute);
    }
    if (normalAttribute >= 0) {
        glVertexAttribPointer((GLuint) normalAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (4 * 3));
        glEnableVertexAttribArray((GLuint) normalAttribute);
    }
    if (texCoordAttribute >= 0) {
        glVertexAttribPointer((GLuint) texCoordAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (4 * 6));
        glEnableVertexAttribArray((GLuint) texCoordAttribute);
    }
}

void Mesh::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    if (positionAttribute >= 0) glDisableVertexAttribArray((GLuint) positionAttribute);
    if (normalAttribute >= 0) glDisableVertexAttribArray((GLuint) normalAttribute);
    if (texCoordAttribute >= 0) glDisableVertexAttribArray((GLuint) texCoordAttribute);
}

void Mesh::setPositionAttribute(GLint attribute) {
    positionAttribute = attribute;
}

void Mesh::setNormalAttribute(GLint attribute) {
    normalAttribute = attribute;
}

void Mesh::setTexCoordAttribute(GLint attribute) {
    texCoordAttribute = attribute;
}

void Mesh::resize(uint32_t numberOfVertices, uint32_t numberOfFaces) {
    vertices.resize(numberOfVertices);
    faces.resize(numberOfFaces);
}

void Mesh::draw() {
    GLsizei size = (GLsizei) (faces.size() * 4);
    glDrawElements(GL_QUADS, size, GL_UNSIGNED_INT, 0);
    int t = sizeof(Eigen::Vector3f);
    int m = sizeof(Vertex);
    int s = 0;
}

void Mesh::calculateNormals() {
    for (auto &it : vertices) {
        it.avgFactor = 0;
    }

    for (auto &face : faces) {
        // based on https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal
        auto &v1 = vertices[face.vertex1].position;
        auto &u = vertices[face.vertex2].position - v1;
        auto &v = vertices[face.vertex3].position - v1;

        Eigen::Vector3f normal = {
                (u[1] * v[2]) - (u[2] * v[1]),
                (u[2] * v[0]) - (u[0] * v[2]),
                (u[0] * v[1]) - (u[1] * v[0]),
        };

        applyNormal(face.vertex1, normal);
        applyNormal(face.vertex2, normal);
        applyNormal(face.vertex3, normal);
        applyNormal(face.vertex4, normal);
    }
}

void Mesh::applyNormal(uint64_t index, const Eigen::Vector3f &normal) {
    auto &vertex = vertices[index];
    int avgFactor = vertex.avgFactor;
    vertex.normal = vertex.normal * avgFactor + normal * (avgFactor + 1);
    vertex.avgFactor++;
}
