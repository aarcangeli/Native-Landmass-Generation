#include <cassert>
#include "Mesh.h"

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
