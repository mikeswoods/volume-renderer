#include <algorithm>
#include <iostream>
#include <limits>
#include "BV.h"
#include "Utils.h"

////////////////////////////////////////////////////////////////////////////////
// Simple bounding box
////////////////////////////////////////////////////////////////////////////////

BoundingBox::BoundingBox()
{ 
    this->p1 = P::AT_INFINITY;
    this->p2 = P::AT_INFINITY;
    this->c  = P::AT_INFINITY;
}

BoundingBox::BoundingBox(const P& p1, const P& p2)
{ 
    this->p1 = p1;
    this->p2 = p2;
    this->c  = P((x(p1) + x(p2)) / 2.0f
                ,(y(p1) + y(p2)) / 2.0f
                ,(z(p1) + z(p2)) / 2.0f);
}

BoundingBox::BoundingBox(const BoundingBox& other)
{ 
    this->p1 = other.p1;
    this->p2 = other.p2;
    this->c  = other.c;
}

// Create a new bounding box instance from a given center and radius
BoundingBox BoundingBox::fromCenter(const P& center, float radius)
{
    return BoundingBox(P(x(center) - radius, y(center) - radius, z(center) - radius)
                      ,P(x(center) + radius, y(center) + radius, z(center) + radius));
}

// Like BoundingBox::isHit(const Ray& ray, P& entered, P& exited), except this
// method only returns true or false if the given ray intersects this bounding
// volume
bool BoundingBox::isHit(const Ray& ray)
{
    P ignore;
    return this->isHit(ray, ignore, ignore);
}

// Tests if a ray intersects with this bounding box volume, returning true if
// so, and setting points entered and exited to the respective points the ray
// entered and exited the volume
bool BoundingBox::isHit(const Ray& ray, P& entered, P& exited)
{
    float xd = x(ray.direction);
    float yd = y(ray.direction);
    float zd = z(ray.direction);

    if (xd == 0) {
        xd = FLT_EPSILON;
    }
    if (yd == 0) {
        yd = FLT_EPSILON;
    }
    if (zd == 0) {
        zd = FLT_EPSILON;
    }

    float x1 = (x(this->p1) - x(ray.origin)) / xd;
    float x2 = (x(this->p2) - x(ray.origin)) / xd;
    float y1 = (y(this->p1) - y(ray.origin)) / yd;
    float y2 = (y(this->p2) - y(ray.origin)) / yd;
    float z1 = (z(this->p1) - z(ray.origin)) / zd;
    float z2 = (z(this->p2) - z(ray.origin)) / zd;

    if (x1 > x2) {
        swap(x1,x2);
    }
    if (y1 > y2) {
       swap(y1,y2);
    }
    if (z1 > z2) {
        swap(z1,z2);   
    }

    float near = max(x1,max(y1,z1));
    float far  = min(x2,min(y2,z2));

    if (near > far || far < 0) {
        return false;
    }

    entered = ray.origin + (ray.direction * near);
    exited  = ray.origin + (ray.direction * far);

    return true;
}

bool BoundingBox::isInside(const P& test)
{
    return Utils::inBounds(test, this->p1, this->p2);
}

std::ostream& operator<<(std::ostream& s, const BoundingBox& bounds)
{
    return s << "BoundingBox { " << bounds.p1 << " ; " << bounds. p2 << " }";
}
