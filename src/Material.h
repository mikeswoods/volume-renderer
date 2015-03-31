#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "R3.h"

////////////////////////////////////////////////////////////////////////////////
// Simple material
////////////////////////////////////////////////////////////////////////////////

// Forward declaration for Color needed below:
class Color;

class Material
{
    public:
        // Calculate the color at a given position on the surface of the
        // implementing object relative to some arbitrary origin
        virtual Color colorAt(const P& position, const P& origin) = 0;
};

#endif
