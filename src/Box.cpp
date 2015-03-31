#include <algorithm>
#include <limits>
#include <iostream>
#include "Context.h"
#include "Box.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Parametric box object
////////////////////////////////////////////////////////////////////////////////

Box::Box(const P& p1, const P& p2, Material* material) : 
    StdObject(BoundingBox(p1,p2), material)
{
    this->p1 = p1;
    this->p2 = p2;
}

Box::Box(const Box& other) : 
    StdObject(BoundingBox(p1,p2), other.material)
{
    this->p1 = other.p1;
    this->p2 = other.p2;
}

std::ostream& operator<<(std::ostream& s, const Box& box)
{
    return s  << "Box {"             << std::endl
              << "  p1 = " << box.p1 << std::endl
              << "  p2 = " << box.p2 << std::endl
              << "}";
}

////////////////////////////////////////////////////////////////////////////////

bool Box::intersects(const Ray& ray, const RenderContext& ctx, Hit& hit)
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
        //cout << "NEAR: " << near << ", FAR: " << far << endl;
        return false;
    }

    P entered = ray.origin + (ray.direction * near);

    hit.color = this->getMaterial()->colorAt(entered, this->bounds.center());

    return true;
}
