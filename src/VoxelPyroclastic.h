#ifndef _VOXEL_PYROCLASTIC
#define _VOXEL_PYROCLASTIC

#include "Voxel.h"

class VoxelPyroclastic : public VoxelBuffer
{
    protected:
        float radius, scale;
    
    public:
        VoxelPyroclastic(float radius
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
