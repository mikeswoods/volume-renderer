#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <cmath>
#include <stdexcept>
#include "R3.h"
#include "Utils.h"

#if defined(WINVER) || defined(_WIN32) || defined(_WIN64)
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
#endif

using namespace std;

// Return the current working directory. This solution was adapted from
// the answer on Stackoverflow at http://stackoverflow.com/a/145309
string Utils::cwd(const std::string& relFile = "")
{
    // Determine the current working directory. Any texture files will be
    // loaded relative to  it
    char cwdBuffer[FILENAME_MAX];

    if (!GetCurrentDir(cwdBuffer, sizeof(cwdBuffer))) {
        throw runtime_error("Couldn't get current working directory!");
    }

    string workingDir = string(cwdBuffer);

    if (relFile == "") {
        return workingDir;
    } else {
        ostringstream is;
        is << workingDir << "/" << relFile;
        return is.str();
    }
}

// Adapted from http://www.toptip.ca/2010/03/trim-leading-or-trailing-white-spaces.html
string Utils::trim(const string& s)
{
    string sCopy(s);

    size_t p = sCopy.find_first_not_of(WHITESPACE_CHARS);
    sCopy.erase(0, p);

    p = sCopy.find_last_not_of(WHITESPACE_CHARS);
    if (string::npos != p) {
        sCopy.erase(p+1);
    }

    return sCopy;
}

float Utils::clamp(float n, float lo, float hi)
{
    return min(max(n,lo),hi); 
}

float Utils::unitClamp(float n)
{
    return min(max(n, 0.0f), 1.0f); 
}

bool Utils::inBounds(const P& test, const P& p, const P& q)
{
    return    (x(test) >= min(x(p),x(q)) && x(test) <= max(x(p),x(q))) 
           && (y(test) >= min(y(p),y(q)) && y(test) <= max(y(p),y(q))) 
           && (z(test) >= min(z(p),z(q)) && z(test) <= max(z(p),z(q)));
}

float Utils::reRange(float value, float a0, float a1, float b0, float b1)
{
    // Adapted from http://stackoverflow.com/a/12413880
    return ((value - a0) / (a1 - a0)) * (b1 - b0) + b0;
}

float Utils::unitRange(float value, float lo, float hi)
{
    return reRange(value, lo, hi, 0.0f, 1.0f);
}

float Utils::lerp(float v1, float v2, float t)
{
    return ((1.0f - t) * v1) + (t * v2);   
}

float Utils::trilerp(float xd, float yd, float zd
                    ,float v000, float v001
                    ,float v010, float v011
                    ,float v100, float v101
                    ,float v110, float v111)
{
    float c00 = lerp(v000, v100, xd);
    float c10 = lerp(v010, v110, xd);
    float c01 = lerp(v001, v101, xd);
    float c11 = lerp(v011, v111, xd);
    float c0  = lerp(c00, c10, yd);
    float c1  = lerp(c01, c11, yd);
    return lerp(c0, c1, zd);
}
