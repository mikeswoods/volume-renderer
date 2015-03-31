#ifndef _LIGHT_H
#define _LIGHT_H

#include <iostream>

// Forward declarations:
class P;
class Color;

////////////////////////////////////////////////////////////////////////////////
// Point light definition
////////////////////////////////////////////////////////////////////////////////

class Light
{
    protected:
        P position;
        Color color;

    public:
        Light() {};
        Light(const P& position, const Color& color);
        Light(const Light &other);

        const P& getPosition() const     { return this->position; }
        const Color& getColor() const { return this->color; }

        friend std::ostream& operator<<(std::ostream& s, const Light& light);
};

#endif
