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
    glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(Vertex), verticies.data(), GL_DYNAMIC_DRAW);
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
    verticies.resize(numberOfVertices);
    faces.resize(numberOfFaces);
}

void Mesh::draw() {
    GLsizei size = (GLsizei) (faces.size() * 4);
    glDrawElements(GL_QUADS, size, GL_UNSIGNED_INT, 0);
}
