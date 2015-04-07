#include <cassert>
#include <stdexcept>
#include "Primitive.h"

/******************************************************************************/

using namespace std;
using namespace glm;

/******************************************************************************/

Primitive::Primitive(glm::ivec3 _gridDim
                    ,const BoundingBox& _bounds) :
    deferDimensions(false),
    gridDim(_gridDim),
    bounds(_bounds),
    material(Color::BLACK)
{
    this->setDimensions(_gridDim);
}

Primitive::Primitive(const BoundingBox& _bounds) :
    deferDimensions(true),
    gridDim(ivec3(-1, -1, -1)),
    voxelDim(ivec3(-1, -1, -1)),
    bounds(_bounds),
    material(Color::BLACK)
{

}

Primitive::Primitive(glm::ivec3 _gridDim
                    ,const BoundingBox& _bounds
                    ,std::shared_ptr<Material> _material) :
    deferDimensions(false),
    gridDim(_gridDim),
    bounds(_bounds),
    material(_material ? _material : Color::BLACK)
{ 
    this->setDimensions(_gridDim);
}

Primitive::Primitive(const BoundingBox& _bounds, std::shared_ptr<Material> _material) :
    deferDimensions(true),
    gridDim(ivec3(-1, -1, -1)),
    voxelDim(ivec3(-1, -1, -1)),
    bounds(_bounds),
    material(_material ? _material : Color::BLACK)
{

}

Primitive::Primitive(const Primitive& other)
{
    this->deferDimensions = other.deferDimensions;
    this->gridDim         = other.gridDim;
    this->voxelDim        = other.voxelDim;
    this->bounds          = other.bounds;
    this->material        = other.material;
}

glm::fvec3 Primitive::computeVoxelDims() const
{
    return glm::fvec3(static_cast<float>(this->bounds.width()) / static_cast<float>(this->gridDim.x)
                     ,static_cast<float>(this->bounds.height()) / static_cast<float>(this->gridDim.y)
                     ,static_cast<float>(this->bounds.depth()) / static_cast<float>(this->gridDim.z));
}

const glm::ivec3& Primitive::getDimensions() const
{ 
    if (this->deferDimensions) {
        throw runtime_error("Dimensions not explicitly set yet. Call VoxelBuffer::setDimensions()");
    }

    return this->gridDim;
}

void Primitive::setDimensions(glm::ivec3 dim)
{
    this->deferDimensions = false;
    this->gridDim         = dim;
    this->voxelDim        = this->computeVoxelDims();
}
