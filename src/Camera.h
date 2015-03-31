#ifndef _CAMERA_H
#define _CAMERA_H

#include <iostream>
#include "R3.h"
#include "Ray.h"

////////////////////////////////////////////////////////////////////////////////
// General camera definition
////////////////////////////////////////////////////////////////////////////////

class Camera
{
    private:
        void calibrateViewPlane();

    protected:
        P position;
        V viewDir, up;
        V u, v, w;     // Basis vectors
        float fov, aspectRatio, phi, theta;

        V viewPlaneX,viewPlaneY;
        P midpoint;

    public:
        Camera() {};
        Camera(const P &position, const V &viewDir, const V &up, float fov, float aspectRatio);
        Camera(const P &position, const V &viewDir, float fov, float aspectRatio);
        Camera(const P &position, const P &lookAt, float fov, float aspectRatio);
        Camera(const Camera &other);

        void setPosition(const P& position);
        void setViewDir(const V& viewDir);
        void setUp(const V& up);
        void setFOV(float fov);
        void setAspectRatio(float aspectRatio);

        P ndc2World(float x, float y) const;
        P screen2World(float x, float y, float w, float h) const;

        Ray spawnRay(float x, float y) const;
        Ray spawnRay(float x, float y, float w, float h) const;

        friend std::ostream& operator<<(std::ostream& s, const Camera& c);
};

#endif
