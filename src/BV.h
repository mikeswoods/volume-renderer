#ifndef _BOUNDING_VOLUME
#define _BOUNDING_VOLUME

#include <algorithm>
#define _USE_MATH_DEFINES
#include <cmath>
#include <limits>
#include <iostream>
#include "R3.h"
#include "Ray.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Simple bounding box
////////////////////////////////////////////////////////////////////////////////

class BoundingBox
{
    protected:
        P p1,p2,c;

    public:
        BoundingBox();
        BoundingBox(const P& _p1, const P& _p2);
        BoundingBox(const BoundingBox& other);

        static BoundingBox fromCenter(const P& center, float radius);
        
        const P& getP1() const   { return this->p1; }
        const P& getP2() const   { return this->p2; }

        const P& center() const { return this->c; }
        float width() const     { return std::abs(x(this->p1) - x(this->p2)); }
        float height() const    { return std::abs(y(this->p1) - y(this->p2)); }
        float depth() const     { return std::abs(z(this->p1) - z(this->p2)); }

        bool isHit(const Ray& ray);
        bool isHit(const Ray& ray, P& entered, P& exited);
        bool isInside(const P& test);

        friend std::ostream& operator<<(std::ostream& s, const BoundingBox& bounds);
};

#endif
