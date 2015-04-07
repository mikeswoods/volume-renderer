#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <memory>
#include <glm/glm.hpp>
#include "BV.h"
#include "Intersect.h"
#include "Material.h"

/*******************************************************************************
 * Standard world object type that has the following properties: 
 * - has a material
 * - has grid dimensions (width, height, depth)
 * - intersectable
 ******************************************************************************/

class Primitive : public Intersect
{
    protected:
        bool deferDimensions;
        glm::ivec3 gridDim;
        glm::fvec3 voxelDim;
        BoundingBox bounds;
        std::shared_ptr<Material> material;

        glm::fvec3 computeVoxelDims() const;

    public:
        Primitive(glm::ivec3 _gridDim, const BoundingBox& _bounds);
        Primitive(const BoundingBox& _bounds);
        Primitive(glm::ivec3 _gridDim, const BoundingBox& _bounds, std::shared_ptr<Material> _material);
        Primitive(const BoundingBox& _bounds, std::shared_ptr<Material> _material);
        Primitive(const Primitive& other);

        virtual ~Primitive() { }

        const glm::ivec3& getDimensions() const;
        void setDimensions(glm::ivec3 dim);
        bool hasLoadedDimensions() const { return !this->deferDimensions; }
        float getVoxelWidth()  const     { return this->voxelDim.x; }
        float getVoxelHeight() const     { return this->voxelDim.y; }
        float getVoxelDepth()  const     { return this->voxelDim.z; }

        // Abstract methods:
        virtual std::string getTypeName() const = 0;

        const BoundingBox& getBoundingBox() const { return this->bounds; }
        std::shared_ptr<Material> getMaterial() const { return this->material; }
};

#endif
