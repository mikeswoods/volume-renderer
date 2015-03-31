#ifndef _VOXEL_CLOUD
#define _VOXEL_CLOUD

#include "Voxel.h"

class VoxelCloud : public VoxelBuffer
{
    protected:
        float radius, scale;
    
    public:
        VoxelCloud(float radius
                  ,float scale
                  ,int xDim
                  ,int yDim
                  ,int zDim
                  ,const BoundingBox& bounds
                  ,Material* material
                  ,int seed
                  ,int octaves
                  ,float freq
                  ,float amp);

        float getScale()  { return this->scale; };
        float getRadius() { return this->radius; };
};

#endif
