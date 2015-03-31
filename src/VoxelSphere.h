#ifndef _VOXEL_SPHERE
#define _VOXEL_SPHERE

#include "Voxel.h"

class VoxelSphere : public VoxelBuffer
{
    protected:
        float radius, scale;

    public:
        VoxelSphere(float radius
                   ,float scale
                   ,int xDim
                   ,int yDim
                   ,int zv
                   ,const BoundingBox& bounds
                   ,Material* material);

        float getScale()  { return this->scale; };
        float getRadius() { return this->radius; };
};

#endif
