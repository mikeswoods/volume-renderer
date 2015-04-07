#include <cassert>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <limits>
#include <list>
#include "Ray.h"
#include "Context.h"
#include "Color.h"
#include "Light.h"
#include "Primitive.h"
#include "Utils.h"
#include "Voxel.h"

/******************************************************************************/

#define MARCH_EPSILON 1.0e-4f

/******************************************************************************/

class BoundingBox;

/******************************************************************************/

using namespace std;
using namespace Utils;
using namespace glm;

/******************************************************************************/

ostream& operator<<(ostream &s, const Voxel &v)
{
    return s << "{ light = " << v.light  << ", density = " << v.density << " }";
}

/******************************************************************************/

Voxel::Voxel(float _density) :
    density(_density)
{ 
    fill(this->light, this->light + MAX_LIGHTS, -1.0f);
}

Voxel::Voxel(const Voxel& other) :
    density(other.density)
{
    for (int i = 0; i < MAX_LIGHTS; i++) {
        this->light[i] = other.light[i];
    }
}

Voxel& Voxel::operator=(const Voxel& other)
{
    if (this == &other) {
        return *this;
    }

    this->density = other.density;
    for (int i = 0; i < MAX_LIGHTS; i++) {
        this->light[i] = other.light[i];
    }

    return *this;
}

/******************************************************************************/

/**
 *
 */
VoxelBuffer::VoxelBuffer(ivec3 _dim
                        ,const BoundingBox& _bounds
                        ,std::shared_ptr<Material> _material) :
    Primitive(_dim, _bounds, _material)
{
    this->buffer = make_shared<vector<Voxel> >();
    this->buffer->resize(this->gridDim.x * this->gridDim.y * this->gridDim.z);
}

/**
 *
 */
VoxelBuffer::VoxelBuffer(ivec3 _dim
                        ,shared_ptr<vector<Voxel> > _buffer
                        ,const BoundingBox& _bounds
                        ,std::shared_ptr<Material> _material) :
    Primitive(_dim, _bounds, _material)
{
    assert(_buffer);
}

/**
 *
 */
VoxelBuffer::VoxelBuffer(shared_ptr<vector<Voxel> > _buffer
                        ,const BoundingBox& _bounds
                        ,std::shared_ptr<Material> _material) :
    Primitive(_bounds, _material)
{
    assert(_buffer);
    this->buffer = _buffer;
}

VoxelBuffer::VoxelBuffer(const VoxelBuffer& other) :
    Primitive(other),
    buffer(other.buffer)
{

}

VoxelBuffer::~VoxelBuffer()
{

}

/*******************************************************************************
 * Dimensioning
 ******************************************************************************/

/**
 *
 */
bool VoxelBuffer::checkBufferSize(const glm::ivec3& dim, std::shared_ptr<std::vector<Voxel> > buffer) const
{
    return static_cast<unsigned int>(dim.x * dim.y * dim.z) == buffer->size();
}

/*******************************************************************************
 * Indexing and assignment operations
 ******************************************************************************/

Voxel VoxelBuffer::operator()(int i, int j, int k) const 
{
    return (*this->buffer)[sub2ind(i, j, k)];
}

Voxel& VoxelBuffer::operator()(int i, int j, int k)
{
    return (*this->buffer)[sub2ind(i, j, k)];
}

Voxel VoxelBuffer::operator[](int i) const
{
    return (*this->buffer)[i];
}

Voxel& VoxelBuffer::operator()(int i)
{
    return (*this->buffer)[i];
}

/*******************************************************************************
 * Intersection
 ******************************************************************************/

bool VoxelBuffer::intersects(const Ray& ray, const RenderContext& context, Hit& hit)
{
    // Only proceed if the grid dimensions of the voxel buffer have been
    // explictly set:
    assert(this->hasLoadedDimensions());

    P entered, exited;

    if (!this->bounds.isHit(ray, entered, exited)) {
        return false;
    }

    RayMarch rm       = rayMarch(context, *this, entered, exited);
    hit.color         = rm.color;
    hit.transmittance = rm.transmittance;

    return true;
}

/**
 * Sets center to the center point of the voxel the point p falls within. 
 * If this method returns false, point o does not fall within a voxel
 */
bool VoxelBuffer::center(const P& p, P& center) const
{
    assert(this->hasLoadedDimensions());

    int i,j,k;

    if (!this->positionToIndex(p, i, j, k)) {
        return false;
    }

    auto p1   = this->bounds.getP1();
    auto p2   = this->bounds.getP2();
    float dx  = (x(p2) - x(p1)) / static_cast<float>(this->gridDim.x);
    float dy  = (y(p2) - y(p1)) / static_cast<float>(this->gridDim.y);
    float dz  = (z(p2) - z(p1)) / static_cast<float>(this->gridDim.z);
    float dx2 = 0.5f * dx; 
    float dy2 = 0.5f * dy;
    float dz2 = 0.5f * dz; 

    center = P(x(p1) + dx2 + (dx * static_cast<float>(i))
              ,y(p1) + dy2 + (dy * static_cast<float>(j))
              ,z(p1) + dz2 + (dz * static_cast<float>(k)));

    return true;
}

/**
 * Sets center to the center point of the voxel the point p falls within. 
 * If this method returns false, the voxel at (i,j,k) does not exist
 */
bool VoxelBuffer::center(int i, int j, int k, P& center) const
{
    assert(this->hasLoadedDimensions());

    if (!this->valid(i, j, k)) {
        return false;
    }

    auto p1   = this->bounds.getP1();
    auto p2   = this->bounds.getP2();
    float dx  = (x(p2) - x(p1)) / static_cast<float>(this->gridDim.x);
    float dy  = (y(p2) - y(p1)) / static_cast<float>(this->gridDim.y);
    float dz  = (z(p2) - z(p1)) / static_cast<float>(this->gridDim.z);
    float dx2 = 0.5f * dx; 
    float dy2 = 0.5f * dy;
    float dz2 = 0.5f * dz; 

    center = P(x(p1) + dx2 + (dx * static_cast<float>(i))
              ,y(p1) + dy2 + (dy * static_cast<float>(j))
              ,z(p1) + dz2 + (dz * static_cast<float>(k)));

    return true;
}

/**
 * Returns the (i,j,k) coordinates of the voxel point p falls within. If this
 * method returns false, point p is not within a voxel
 */
bool VoxelBuffer::positionToIndex(const P& p, int& i, int& j, int& k) const
{
    assert(this->hasLoadedDimensions());

    auto p1  = this->bounds.getP1();
    auto p2  = this->bounds.getP2();
    float dx = unitRange(x(p), x(p1), x(p2));
    float dy = unitRange(y(p), y(p1), y(p2));
    float dz = unitRange(z(p), z(p1), z(p2));

    // Needed for rounding errors: if the number is smaller than the threshold,
    // it just becomes 0.0f
    if (fabs(dx) < MARCH_EPSILON) {
        dx = 0.0f;
    }
    if (fabs(dy) < MARCH_EPSILON) {
        dy = 0.0f;
    }
    if (fabs(dz) < MARCH_EPSILON) {
        dz = 0.0f;
    }

    float xLoc  = dx * ((static_cast<float>(this->gridDim.x)) - this->getVoxelWidth());
    float yLoc  = dy * ((static_cast<float>(this->gridDim.y)) - this->getVoxelHeight());
    float zLoc  = dz * ((static_cast<float>(this->gridDim.z)) - this->getVoxelDepth());
    float xCell = static_cast<int>(xLoc);
    float yCell = static_cast<int>(yLoc);
    float zCell = static_cast<int>(zLoc);

    if (!this->valid(xCell, yCell, zCell)) {
        return false;
    }

    i = xCell;
    j = yCell;
    k = zCell;

    return true;
}

/**
 * Gets the trilinearly interpolated density for the given position
 */
float VoxelBuffer::getInterpolatedDensity(const P& p) const
{
    assert(this->hasLoadedDimensions());

    auto p1  = this->bounds.getP1();
    auto p2  = this->bounds.getP2();
    float dx = unitRange(x(p), x(p1), x(p2));
    float dy = unitRange(y(p), y(p1), y(p2));
    float dz = unitRange(z(p), z(p1), z(p2));

    float xLoc = dx * ((float)this->gridDim.x - 1);
    float xWeight = xLoc - floor(xLoc); 
    int x1 = static_cast<int>(xLoc);
    int x2 = static_cast<int>(ceil(xLoc));

    float yLoc = dy * ((float)this->gridDim.y - 1);
    float yWeight = yLoc - floor(yLoc); 
    int y1 = static_cast<int>(yLoc);
    int y2 = static_cast<int>(ceil(yLoc));    

    float zLoc = dz * ((float)this->gridDim.z - 1);
    float zWeight = zLoc - floor(zLoc); 
    int z1 = static_cast<int>(zLoc);
    int z2 = static_cast<int>(ceil(zLoc));

    float x1y1z1D = 0.0f;
    float x1y1z2D = 0.0f;
    float x1y2z1D = 0.0f;
    float x1y2z2D = 0.0f;
    float x2y1z1D = 0.0f;
    float x2y1z2D = 0.0f;
    float x2y2z1D = 0.0f;
    float x2y2z2D = 0.0f;

    if (this->valid(x1,y1,z1)) {
        x1y1z1D = (*this)(x1,y1,z1).density;
    }
    if (this->valid(x1,y1,z2)) {
        x1y1z2D = (*this)(x1,y1,z2).density;
    }
    if (this->valid(x1,y2,z1)) {
        x1y2z1D = (*this)(x1,y2,z1).density;
    }
    if (this->valid(x1,y2,z2)) {
        x1y2z2D = (*this)(x1,y2,z2).density;
    }
    if (this->valid(x2,y1,z1)) {
        x2y1z1D = (*this)(x2,y1,z1).density;
    }
    if (this->valid(x2,y1,z2)) {
        x2y1z2D = (*this)(x2,y1,z2).density;
    }
    if (this->valid(x2,y2,z1)) {
        x2y2z1D = (*this)(x2,y2,z1).density;
    }
    if (this->valid(x2,y2,z2)) {
        x2y2z2D = (*this)(x2,y2,z2).density;
    }

    return trilerp(xWeight, yWeight, zWeight, x1y1z1D, x1y1z2D, x1y2z1D,
                   x1y2z2D, x2y1z1D, x2y1z2D, x2y2z1D, x2y2z2D) / 3.0f;
}

/**
 * Convert a 3D index to a linear index
 */
int VoxelBuffer::sub2ind(int i, int j, int k) const
{
    assert(this->hasLoadedDimensions());

    return i + (j * this->gridDim.x) + k * (this->gridDim.x * this->gridDim.y);
}

/** 
 * Convert a linear index to a 3D index
 */
void VoxelBuffer::ind2sub(int w, int& i, int& j, int& k) const
{
    assert(this->hasLoadedDimensions());

    i = w % this->gridDim.x;
    j = (w / this->gridDim.x) % this->gridDim.y;
    k = w / (this->gridDim.y * this->gridDim.x); 
}

/**
 * Tests if an index is valid
 */
bool VoxelBuffer::valid(int i, int j, int k) const
{
    assert(this->hasLoadedDimensions());

    return (i >= 0 && i < this->gridDim.x) &&
           (j >= 0 && j < this->gridDim.y) &&
           (k >= 0 && k < this->gridDim.z);
}

ostream& operator<<(ostream &s, const VoxelBuffer &vb)
{
    s << "VoxelBuffer[" << vb.gridDim.x << "]"   <<
                    "[" << vb.gridDim.y << "]"   <<
                    "[" << vb.gridDim.z << "] {" << endl;
    int q = 0, w = 0;

    for (int k=0; k<vb.gridDim.z; k++) {
        for (int j=0; j<vb.gridDim.y; j++) {
            for (int i=0; i<vb.gridDim.x; i++) {
                int ii,jj,kk;
                w = vb.sub2ind(i,j,k);
                vb.ind2sub(w, ii, jj, kk);
                s << q++ << "\t[(" << i << "," << j << "," << k << ")" 
                         <<  " => (" << ii << "," << jj << "," << kk <<")"
                         << " => " << (*vb.buffer)[w] 
                  << endl;
            }
        }
    }
    return s << "}";
}

/*******************************************************************************
 * Raymarching implementation using Beer's law for transmittance
 ******************************************************************************/

float Q(const VoxelBuffer& vb
       ,float kappa
       ,float step
       ,int iterations
       ,const P& X
       ,const V& N)
{
    assert(vb.hasLoadedDimensions());

    int i = -1;
    int j = -1;
    int k = -1;

    // Done or outside of the volume
    if (!vb.positionToIndex(X, i, j, k) || iterations <= 0) {
        return 1.0f;
    }

    return exp(-kappa * step * vb(i, j, k).density) * 
           Q(vb, kappa, step, iterations - 1, X + N, N);
}

RayMarch rayMarch(const RenderContext& context
                 ,const VoxelBuffer& vb
                 ,const P& start
                 ,const P& end
                 ,float (*densityFunction)(const Voxel& voxel, const P& X, void* densityData)
                 ,void* densityData)
{
    float step        = context.getStep();
    float kappa       = 1.0f;
    float T           = 1.0f;
    bool interpolate  = context.getInterpolation();
    auto material     = vb.getMaterial();
    auto lights       = context.getLights();
    auto accumColor   = Color(0.0f, 0.0f, 0.0f);

    P X;
    V N;
    int iterations = traverse(step, MARCH_EPSILON, start, end, X, N);

    for (int i=0; i<iterations; i++, X += N) {

        int vi = -1;
        int vj = -1;
        int vk = -1;

        if (!vb.positionToIndex(X, vi, vj, vk)) {
            break;
        }

        auto voxel = vb(vi, vj, vk);

        // If the density function is provided, use it
        float density = densityFunction == nullptr 
            ? voxel.density 
            : densityFunction(voxel, X, densityData);
        
        if (interpolate) {
            density = vb.getInterpolatedDensity(X);
        }

        float deltaT      = exp(-kappa * step * density);
        float attenuation = (1.0f - deltaT) / kappa;

        T *= deltaT;

        P center;
        vb.center(X, center);

        P LX;
        V LN;
        float offset = (2.0f * step) + MARCH_EPSILON;

        // For every light in the scene:
        auto li = lights.begin();

        for (int k=0; li != lights.end(); li++, k++) {

            auto light = *li;

            int stepsToLight = traverse(step, offset, center, light->getPosition(), LX, LN);

            if (voxel.light[k] < 0.0f) {
                voxel.light[k] = Q(vb, kappa, step, stepsToLight, LX, LN);
            }

            accumColor += light->getColor() * 
                          material->colorAt(X, vb.getBoundingBox().center()) * 
                          attenuation * 
                          T * 
                          voxel.light[k];
        }
    }

    return RayMarch(accumColor, T);
}

/******************************************************************************/
