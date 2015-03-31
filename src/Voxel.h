#ifndef _VOXEL
#define _VOXEL

#include <algorithm>
#include <iostream>
#include <list>
#include "Context.h"
#include "Color.h"
#include "StdObject.h"

using namespace std;

#define MAX_LIGHTS 5

/*******************************************************************************
 * Individual voxel
 ******************************************************************************/

typedef struct Voxel
{
    float light[MAX_LIGHTS];
    float density;

    Voxel(float density = 0.0f)
    { 
        this->density = density;
        fill(this->light, this->light + MAX_LIGHTS, -1.0f);
    };

} Voxel;

/*******************************************************************************
 * Voxel buffer
 ******************************************************************************/

class VoxelBuffer : public StdObject
{
    private:
        int sub2ind(int i, int j, int k) const;
        void ind2sub(int w, int& i, int& j, int& k) const;
        bool valid(int i, int j, int k) const;

    protected:
        int xDim, yDim, zDim;
        float vWidth, vHeight, vDepth;
        Voxel* buffer;
        Material* material;

    public:
        VoxelBuffer(int x, int y, int z, const BoundingBox& bounds, Material* material);
        ~VoxelBuffer();

        int getX() const { return this->xDim; }
        int getY() const { return this->yDim; }
        int getZ() const { return this->zDim; }

        virtual Material* getMaterial() const;

        float getVoxelWidth()  const { return this->vWidth; }
        float getVoxelHeight() const { return this->vHeight; }
        float getVoxelDepth()  const { return this->vDepth; }

        bool center(const P& p, P& center) const;
        bool center(int i, int j, int k, P& center) const;
        bool positionToIndex(const P& p, int& i, int& j, int& k) const;
        Voxel* positionToVoxel(const P& p) const;
        float getInterpolatedDensity(const P& p) const;

        // Indexing and assignment operations

        Voxel* operator() (int i, int j, int k) const;
        Voxel* operator[](int i) const;

        void set(int i, const Voxel& v);
        void set(int i, int j, int k, const Voxel& v);

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
                 ,float (*densityFunction)(Voxel* voxel, const P& X, void* densityData) = NULL
                 ,void* densityData = NULL);

#endif
