#ifndef _STD_OBJECT_H
#define _STD_OBJECT_H

#include "BV.h"
#include "Intersect.h"
#include "Material.h"

////////////////////////////////////////////////////////////////////////////////
// Standard world object type that has the following properties: 
// - has a material
// - intersectable
////////////////////////////////////////////////////////////////////////////////

class StdObject : public Intersect
{
    protected:
        BoundingBox bounds;
        const Material* material;

    public:
        StdObject(const BoundingBox& bounds)
        {
            this->bounds   = bounds;
            this->material = &Color::BLACK;
        }

        StdObject(const BoundingBox& bounds, const Material* material)
        { 
            this->bounds   = bounds;
            this->material = material != NULL ? material : &Color::BLACK;
        }

        // abstract methods:
        virtual std::string getTypeName() const = 0;

        const BoundingBox& getBoundingBox() const { return this->bounds; }
        const Material* getMaterial() const { return this->material; }
};

#endif
