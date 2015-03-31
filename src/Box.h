#ifndef _BOX_H
#define _BOX_H

#include "R3.h"
#include "Context.h"
#include "StdObject.h"

////////////////////////////////////////////////////////////////////////////////
// Parametric box object
////////////////////////////////////////////////////////////////////////////////

class Box : public StdObject
{
    protected:
        P p1,p2;
        Material* material;

    public:
        Box(const P& p1, const P& p2, Material* material);
        Box(const Box& other);

        virtual bool intersects(const Ray& ray, const RenderContext& ctx, Hit& hit);
        
        std::string getTypeName() const { return "Box"; };
        Material* getMaterial() const   { return this->material; }

        friend std::ostream& operator<<(std::ostream& s, const Box& box);
};

#endif
