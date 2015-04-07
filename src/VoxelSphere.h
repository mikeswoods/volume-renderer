#ifndef _VOXEL_SPHERE
#define _VOXEL_SPHERE

#include <memory>
#include "Voxel.h"

class VoxelSphere : public VoxelBuffer
{
    protected:
        float radius, scale;

    public:
        VoxelSphere(float radius
                   ,float scale
                   ,glm::ivec3 dim
                   ,const BoundingBox& bounds
                   ,std::shared_ptr<Material> material);

        float getScale()  { return this->scale; };
        float getRadius() { return this->radius; };
};

#endif
