#include <cassert>
#include "VoxelSphere.h"

VoxelSphere::VoxelSphere(float radius
                        ,float scale
                        ,int xDim
                        ,int yDim
                        ,int zDim
                        ,const BoundingBox& bounds
                        ,Material* material) :
VoxelBuffer(glm::ivec3(xDim, yDim, zDim), bounds, material)
{ 
    this->radius = radius;
    this->scale  = scale;

    P sphereCenter = bounds.center();

    for (int k=0; k<this->dim.z; k++) {

        for (int j=0; j<this->dim.y; j++) {

            for (int i=0; i<this->dim.x; i++) {

                // For each (i,j,k) voxel, find its distance from the center

                P voxelCenter;
                this->center(i, j, k, voxelCenter);

                float d       = dist(voxelCenter, sphereCenter);
                float density = 0.0f;

                if (d <= this->radius) {
                    density = (1.0f - (glm::length(sphereCenter - voxelCenter) / this->radius)) * this->scale;
                    //density = ((this->radius - d) / this->radius) * this->scale; 
                }

                (*this)(i, j, k) = Voxel(density);
            }
        }
    }
}
