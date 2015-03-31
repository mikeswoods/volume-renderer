#ifndef _INTERSECT_H
#define _INTERSECT_H

#include "Ray.h"
#include "Context.h"

////////////////////////////////////////////////////////////////////////////////
// Intersectable object definition
////////////////////////////////////////////////////////////////////////////////

class Intersect
{
    public:
        /** 
         * Allows implementing objects to be tested for intersection with a ray
         *
         * @param ray The ray to test for intersection
         * @param ctx The current rendering context
         * @param hit The ray hit, if any. This value is only valid if
         * intersect returns true
         */
        virtual bool intersects(const Ray& ray, const RenderContext& ctx, Hit& hit) = 0;
};

#endif
