template<class FACE>
void AbstractBuffer<FACE>::init() {
    glGenBuffers(1, &arrayBufferObject);
    glGenBuffers(1, &elementArrayBufferObject);
}


template<class FACE>
void AbstractBuffer<FACE>::refresh() {
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(FACE), faces.data(), GL_DYNAMIC_DRAW);
}

template<class FACE>
void AbstractBuffer<FACE>::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, arrayBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBufferObject);
    // build semantic
    if (positionAttribute >= 0) {
        glVertexAttribPointer((GLuint) positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (4 * 0));
        glEnableVertexAttribArray((GLuint) positionAttribute);
    }
    if (normalAttribute >= 0) {
        glVertexAttribPointer((GLuint) normalAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (4 * 3));
        glEnableVertexAttribArray((GLuint) normalAttribute);
    }
    if (texCoordAttribute >= 0) {
        glVertexAttribPointer((GLuint) texCoordAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (4 * 6));
        glEnableVertexAttribArray((GLuint) texCoordAttribute);
    }
    if (colorAttribute >= 0) {
        glVertexAttribPointer((GLuint) colorAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (4 * 8));
        glEnableVertexAttribArray((GLuint) colorAttribute);
    }
}

template<class FACE>
void AbstractBuffer<FACE>::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    if (positionAttribute >= 0) glDisableVertexAttribArray((GLuint) positionAttribute);
    if (normalAttribute >= 0) glDisableVertexAttribArray((GLuint) normalAttribute);
    if (texCoordAttribute >= 0) glDisableVertexAttribArray((GLuint) texCoordAttribute);
    if (colorAttribute >= 0) glDisableVertexAttribArray((GLuint) colorAttribute);
}

template<class FACE>
void AbstractBuffer<FACE>::setPositionAttribute(GLint attribute) {
    positionAttribute = attribute;
}

template<class FACE>
void AbstractBuffer<FACE>::setNormalAttribute(GLint attribute) {
    normalAttribute = attribute;
}

template<class FACE>
void AbstractBuffer<FACE>::setTexCoordAttribute(GLint attribute) {
    texCoordAttribute = attribute;
}

template<class FACE>
void AbstractBuffer<FACE>::setColorAttribute(GLint attribute) {
    colorAttribute = attribute;
}

template<class FACE>
void AbstractBuffer<FACE>::resize(uint32_t numberOfVertices, uint32_t numberOfFaces) {
    vertices.resize(numberOfVertices);
    faces.resize(numberOfFaces);
}

template<class FACE>
void AbstractBuffer<FACE>::draw() {
    GLsizei size = (GLsizei) (faces.size() * FACE::VERTICES_PER_FACE);
    glDrawElements(FACE::MODE, size, GL_UNSIGNED_INT, 0);
}
