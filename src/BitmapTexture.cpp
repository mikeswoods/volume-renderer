#define _USE_MATH_DEFINES
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cmath>
#include <stdexcept>
#include <stdio.h>
#include <glm/glm.hpp>
#include "Color.h"
#include "BitmapTexture.h"
#include "Utils.h"

using namespace std;

BitmapTexture::BitmapTexture(string fileName)
{
    this->filePath = Utils::cwd(fileName);

    if (!this->texture.load(this->filePath.c_str())) {
        throw runtime_error("Couldn't load texture from " + this->filePath);
    }

    this->width  = static_cast<float>(this->texture.width());
    this->height = static_cast<float>(this->texture.height());
}

// Perform spherical UV mapping based on the given position
Color BitmapTexture::colorAt(const P& position, const P& origin)
{
    V _d = position - origin;
    V d  = glm::normalize(_d);
    //float mag = glm::length(_d);

    // Calculate the spherical UV coordinates. Adapted from 
    // http://en.wikipedia.org/wiki/UV_mapping
    float u    = (float)Utils::unitClamp(0.5f + (atan2f(z(d), x(d)) / (2.0f * (float)M_PI)));
    float v    = (float)Utils::unitClamp(0.5f - (asinf(y(d)) / (float)M_PI));
    int i      = (int)floor(u * (this->width - 1));
    int j      = (int)floor(v * (this->height - 1));
    auto red   = this->texture(i, j, 0, 0); // Red channel
    auto green = this->texture(i, j, 0, 1); // Green channel
    auto blue  = this->texture(i, j, 0, 2); // Blue channel

    return Color(red, green, blue);
}
