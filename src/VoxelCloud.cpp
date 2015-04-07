#include <cassert>
#include <cmath>
#include <algorithm>
#include "perlin.h"
#include "Voxel.h"
#include "VoxelCloud.h"

VoxelCloud::VoxelCloud(float radius
                      ,float scale
                      ,glm::ivec3 dim
                      ,const BoundingBox& bounds
                      ,std::shared_ptr<Material> material
                      ,int seed
                      ,int octaves
                      ,float freq
                      ,float amp) :
    VoxelBuffer(dim, bounds, material)
{
    this->radius = radius;
    this->scale  = scale;

    //clog << "octaves: "<<octaves<<", freq: "<<freq<<", amp: "<<amp<<", seed: "<<seed<<endl;

    P cloudCenter = bounds.center();
    Perlin noise(octaves, freq, amp, seed);

    for (int k=0; k<this->gridDim.z; k++) {

        for (int j=0; j<this->gridDim.y; j++) {

            for (int i=0; i<this->gridDim.x; i++) {

                P voxelCenter;
                this->center(i, j, k, voxelCenter);

                P p           = relative(voxelCenter, cloudCenter);
                float fbm     = noise.Get(x(p), y(p), z(p));
                float factor  = (1.0f - (glm::length(cloudCenter - voxelCenter) / this->radius));
                float density = std::max(0.0f, (fbm + factor) * this->scale);

                (*this)(i, j, k) = Voxel(density);
            }
        }
    }
}
