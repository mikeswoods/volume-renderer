#ifndef _VOXEL
#define _VOXEL

#include <algorithm>
#include <iostream>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Context.h"
#include "Color.h"
#include "StdObject.h"

/******************************************************************************/

#define MAX_LIGHTS 5

/*******************************************************************************
 * Individual voxel
 ******************************************************************************/

class Voxel
{
    public:
        float light[MAX_LIGHTS];
        float density;

    public:
        Voxel(float density = 0.0f);
        Voxel(const Voxel& other);
        Voxel& operator=(const Voxel& other);

        friend std::ostream& operator<<(std::ostream &s, const Voxel &v);
};


/*******************************************************************************
 * Voxel buffer
 ******************************************************************************/

class VoxelBuffer : public StdObject
{
    private:
        int sub2ind(int i, int j, int k) const;
        void ind2sub(int w, int& i, int& j, int& k) const;
        bool valid(int i, int j, int k) const;
        glm::fvec3 computeVoxelDims() const;

    protected:
        glm::ivec3 gridDim; 
        glm::fvec3 voxelDim; 
        std::shared_ptr<std::vector<Voxel> > buffer;
        Material* material;

    public:
        VoxelBuffer(glm::ivec3 dim, const BoundingBox& bounds, Material* material);
        VoxelBuffer(glm::ivec3 dim, std::shared_ptr<std::vector<Voxel> > voxels, const BoundingBox& bounds, Material* material);
        VoxelBuffer(const VoxelBuffer& other);
        virtual ~VoxelBuffer();
        VoxelBuffer& operator=(const VoxelBuffer& other);

        const glm::ivec3& getDimensions() const { return this->gridDim; }

        virtual Material* getMaterial() const;

        float getVoxelWidth()  const { return this->voxelDim.x; }
        float getVoxelHeight() const { return this->voxelDim.y; }
        float getVoxelDepth()  const { return this->voxelDim.z; }

        bool center(const P& p, P& center) const;
        bool center(int i, int j, int k, P& center) const;
        bool positionToIndex(const P& p, int& i, int& j, int& k) const;
        float getInterpolatedDensity(const P& p) const;

        // Indexing and assignment operations

        Voxel operator() (int i, int j, int k) const;
        Voxel& operator() (int i, int j, int k);
        Voxel operator[](int i) const;
        Voxel& operator()(int i);

        // Intersection

        virtual bool intersects(const Ray& ray, const RenderContext& ctx, Hit& hit);

        std::string getTypeName() const { return "VoxelBuffer"; };

        friend std::ostream& operator<<(std::ostream &s, const VoxelBuffer& b);
};

/******************************************************************************/

typedef struct RayMarch {

    Color color;
    float transmittance;

    RayMarch() : 
        transmittance(0.0f)
    { };
    RayMarch(Color _color, float _transmittance) : 
        color(_color), 
        transmittance(_transmittance)
    { };

} RayMarch;

float Q(const VoxelBuffer& vb
       ,float kappa
       ,float step
       ,int iterations
       ,const P& X
       ,const V& N);

RayMarch rayMarch(const RenderContext& ctx
                 ,const VoxelBuffer& vb
                 ,const P& startPosition
                 ,const P& endPosition
                 ,float (*densityFunction)(const Voxel& voxel, const P& X, void* densityData) = NULL
                 ,void* densityData = NULL);

#endif
