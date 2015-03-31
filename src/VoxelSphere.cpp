#include <cassert>
#include "VoxelSphere.h"

VoxelSphere::VoxelSphere(float radius
                        ,float scale
                        ,int xDim
                        ,int yDim
                        ,int zDim
                        ,const BoundingBox& bounds
                        ,Material* material) :
VoxelBuffer(xDim, yDim, zDim, bounds, material)
{ 
    this->radius = radius;
    this->scale  = scale;

    P sphereCenter = bounds.center();

    for (int k=0; k<this->zDim; k++) {

        for (int j=0; j<this->yDim; j++) {

            for (int i=0; i<this->xDim; i++) {

                // For each (i,j,k) voxel, find its distance from the center

                P voxelCenter;
                this->center(i, j, k, voxelCenter);

                float d       = dist(voxelCenter, sphereCenter);
                float density = 0.0f;

                if (d <= this->radius) {
                    density = (1.0f - (glm::length(sphereCenter - voxelCenter) / this->radius)) * this->scale;
                    //density = ((this->radius - d) / this->radius) * this->scale; 
                }

                this->set(i, j, k, Voxel(density));
            }
        }
    }
}
