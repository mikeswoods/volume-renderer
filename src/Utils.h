#ifndef _UTILS_H
#define _UTILS_H

#include "R3.h"

#define WHITESPACE_CHARS " \t\r\n"

namespace Utils {

    // Return the current working directory
    std::string cwd(const std::string& relFile);

    // Trims leading and trailing whitespace from a string 
    std::string trim(const std::string& s);

    // Clamp a value to the range [lo,hi]
    extern float clamp(float n, float lo, float hi);

    // Clamp a value to the unit range [0,1]
    extern float unitClamp(float n);

    // Test if a given point falls with the bounds of the cube formed by
    // vertices p and q
    extern bool inBounds(const P& test, const P& p, const P& q);

    // Converts a number (n) and range [lo,hi] such that lo < n <= hi, to a
    // new number (n') and range [lo',hi'] pwhile preserving n's proportional
    // value to lo and hi
    extern float reRange(float value, float a0, float a1, float b0, float b1);

    // Same as reRange, but rescales to the range [0,1]
    extern float unitRange(float value, float lo, float hi);

    // Linear interpolation
    extern float lerp(float v1, float v2, float t);

    // // TriLinear interpolation
    extern float trilerp(float xd, float yd, float zd
                        ,float v000, float v001
                        ,float v010, float v011
                        ,float v100, float v101
                        ,float v110, float v111);
}

#endif
