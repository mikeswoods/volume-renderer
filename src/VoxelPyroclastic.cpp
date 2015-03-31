#include <cassert>
#include <cmath>
#include <algorithm>
#include "perlin.h"
#include "Voxel.h"
#include "VoxelPyroclastic.h"

VoxelPyroclastic::VoxelPyroclastic(float radius
                                  ,float scale
                                  ,int xDim
                                  ,int yDim
                                  ,int zDim
                                  ,const BoundingBox& bounds
                                  ,Material* material
                                  ,int seed
                                  ,int octaves
                                  ,float freq
                                  ,float amp) :
    VoxelBuffer(xDim, yDim, zDim, bounds, material)
{
    this->radius = radius;
    this->scale  = scale;

    //clog << "octaves: "<<octaves<<", freq: "<<freq<<", amp: "<<amp<<", seed: "<<seed<<endl;

    P cloudCenter = bounds.center();
    Perlin noise(octaves, freq, amp, seed);

    for (int k=0; k<this->zDim; k++) {

        for (int j=0; j<this->yDim; j++) {

            for (int i=0; i<this->xDim; i++) {

                P voxelCenter;
                this->center(i, j, k, voxelCenter);

                P p           = relative(voxelCenter, cloudCenter);
                float fbm     = noise.Get(x(p), y(p), z(p));
                float factor  = glm::length(cloudCenter - voxelCenter) / this->radius;
                float density = std::max(0.0f, this->radius - factor + std::fabs(fbm)) * this->scale;

                this->set(i, j, k, Voxel(density));
            }
        }
    }
}
