#include <iostream>
#include "Color.h"
#include "Light.h"

Light::Light(const Light &other)
{
    this->position = other.position;
    this->color    = other.color;
}

Light::Light(const P& position, const Color& color)
{
    this->position = position;
    this->color    = color;
}

std::ostream& operator<<(std::ostream& s, const Light& light)
{
    return s << "Light {"                              << std::endl
            <<  "  position = " << light.getPosition() << std::endl
            <<  "  color    = " << light.getColor()    << std::endl
            <<  "}";
}
