#include <cassert>
#include "VoxelSphere.h"

VoxelSphere::VoxelSphere(float radius
                        ,float scale
                        ,glm::ivec3 dim
                        ,const BoundingBox& bounds
                        ,std::shared_ptr<Material> material) :
VoxelBuffer(dim, bounds, material)
{ 
    this->radius = radius;
    this->scale  = scale;

    P sphereCenter = bounds.center();

    for (int k=0; k<this->gridDim.z; k++) {

        for (int j=0; j<this->gridDim.y; j++) {

            for (int i=0; i<this->gridDim.x; i++) {

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
