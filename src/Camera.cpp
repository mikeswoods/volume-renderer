#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <glm/glm.hpp>
#include "R3.h"
#include "Ray.h"
#include "Camera.h"
#include "Utils.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// General camera definition
////////////////////////////////////////////////////////////////////////////////

Camera::Camera(const P &position, const V &viewDir, const V &up, float fov, float aspectRatio)
{
    this->position    = position;
    this->viewDir     = viewDir;
    this->up          = glm::normalize(up);
    this->fov         = fov;
    this->phi         = 0.0f;
    this->theta       = 0.0f;
    this->aspectRatio = aspectRatio;

    this->calibrateViewPlane();
}

Camera::Camera(const P &position, const V &viewDir, float fov, float aspectRatio)
{
    this->position    = position;
    this->viewDir     = viewDir;
    this->up          = V(0.0f, 1.0f, 0.0f);
    this->fov         = fov;
    this->phi         = 0.0f;
    this->theta       = 0.0f;
    this->aspectRatio = aspectRatio;

    this->calibrateViewPlane();
}

Camera::Camera(const P &position, const P &lookAt, float fov, float aspectRatio)
{
    this->position    = position;
    this->viewDir     = lookAt - position;
    this->up          = V(0.0f, 1.0f, 0.0f);
    this->fov         = fov;
    this->phi         = 0.0f;
    this->theta       = 0.0f;
    this->aspectRatio = aspectRatio;

    this->calibrateViewPlane();
}

Camera::Camera(const Camera &other)
{
    this->position   = other.position;
    this->viewDir    = other.viewDir;
    this->up         = other.up;
    this->fov        = other.fov;
    this->phi        = other.phi;
    this->theta      = other.theta;
    this->viewPlaneX = other.viewPlaneX;
    this->viewPlaneY = other.viewPlaneY;
    this->midpoint   = other.midpoint;
}

// Update the camera's position
void Camera::setPosition(const P& position)
{
    this->position = position;

    this->calibrateViewPlane();
}

// Update the camera's view direction
void Camera::setViewDir(const V& viewDir)
{
    this->viewDir = viewDir;

    this->calibrateViewPlane();
}

// Update the camera's "up" vector
void Camera::setUp(const V& up)
{
    this->up = up;

    this->calibrateViewPlane();
}

// Updates the FOV Y value of the camera
void Camera::setFOV(float fov)
{
    this->fov = fov;

    this->calibrateViewPlane();
}

// Updates the camera's aspect ratio
void Camera::setAspectRatio(float aspectRatio)
{
    this->aspectRatio = aspectRatio;

    this->calibrateViewPlane();
}

// Convert an (u,v) coordinate in the range {[0,1],[0,1]} to a position in
// R3 space
P Camera::ndc2World(float x, float y) const 
{
    // This is needed since (0,0) corresponds to the bottom-left corner.
    // This inverts (0,0) such that it is in the upper-left corner, since y
    // is int he range [0,1].
    float yPrime = 1.0f - y;
    return this->midpoint + (this->viewPlaneX * ((2.0f * x) - 1.0f))
                          + (this->viewPlaneY * ((2.0f * yPrime) - 1.0f));
}

// Convert an x,y coorindate in the range {[0,screenWidth),[0,screenHeight)} 
// to a position in R3 space
P Camera::screen2World(float x, float y, float w, float h) const
{
    return ndc2World((float)x / (float)w, (float)y / (float)h);
}

// Spawns a ray from the camera's current position (eye) along the 
// vector formed from the eye to the world point in NDC space given by (x,y)
Ray Camera::spawnRay(float x, float y) const
{
    return Ray(this->position, ndc2World(x,y) - this->position);
}

// Spawns a ray from the camera's current position (eye) along the 
// vector formed from the eye to the world point in screen space given by (x,y)
// and (w,h)
Ray Camera::spawnRay(float x, float y, float w, float h) const
{
    return Ray(this->position, screen2World(x,y,w,h) - this->position);
}

std::ostream& operator<<(std::ostream& s, const Camera& c)
{
    return s                                << 
        "Camera {"                          << std::endl <<
        "  position    = " << c.position    << std::endl << 
        "  viewDir     = " << c.viewDir     << std::endl <<
        "  up          = " << c.up          << std::endl <<
        "  fov         = " << c.fov         << std::endl <<
        "  theta       = " << c.theta       << std::endl <<
        "  phi         = " << c.phi         << std::endl <<
        "  aspectRatio = " << c.aspectRatio << std::endl <<
        "  midpoint    = " << c.midpoint    << std::endl << 
        "}";
}

////////////////////////////////////////////////////////////////////////////////

// From slide 163
void Camera::calibrateViewPlane()
{
    // Basis vectors: 
    //   u = up 
    //   v = right
    //   w = forward
    this->w        = this->viewDir;
    this->u        = glm::cross(this->w,this->up);
    this->v        = glm::cross(this->u, this->w);
    this->midpoint = this->position + this->viewDir;

    float Ubar    = glm::length(this->u);
    float Vbar    = glm::length(this->v);
    float Wbar    = glm::length(this->w);

    this->phi   = tanf(this->fov * (float)M_PI / 180.0f);
    this->theta = this->phi * this->aspectRatio;
    
    this->viewPlaneX = (this->u * Wbar * this->theta) / Ubar;    
    this->viewPlaneY = (this->v * Wbar * this->phi) / Vbar;
}
