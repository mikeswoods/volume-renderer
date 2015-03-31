#include "Ray.h"

////////////////////////////////////////////////////////////////////////////////
// General ray definition
////////////////////////////////////////////////////////////////////////////////

Ray::Ray(const P &origin, const V &direction)
{
    this->origin    = origin;
    this->direction = glm::normalize(direction);
}

Ray::Ray(const Ray &other)
{
    this->origin    = other.origin;
    this->direction = glm::normalize(other.direction);
}

std::ostream& operator<<(std::ostream &s, Ray &r)
{
    return s                            << 
        "Ray {"                         << std::endl <<
        "  origin    = " << r.origin    << std::endl << 
        "  direction = " << r.direction << std::endl <<
        "}"                             << std::endl;
}
