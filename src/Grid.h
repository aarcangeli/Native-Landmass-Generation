#ifndef GRID_H
#define GRID_H

#include "Mesh.h"
#include "utils/use-eigen.h"

class Grid : public LineBuffer {
public:
    void generateGrid(uint32_t sizeX, uint32_t sizeZ, float dimension, Eigen::Vector3f color = {0, 0, 0});
    void generateAxes(float dimension);
};


#endif //GRID_H
