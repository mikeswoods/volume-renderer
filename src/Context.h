#ifndef _RENDER_CONTEXT_H
#define _RENDER_CONTEXT_H

#include <list>
#include "Color.h"

// Forward declarations:
class Light;
class StdObject;

////////////////////////////////////////////////////////////////////////////////
// Options and variables relevant to the current rendering context
////////////////////////////////////////////////////////////////////////////////

class RenderContext 
{
    protected:
        float step;                    // Sampling step size
        bool interpolate;              // Enable trilinear interpolation
        std::list<StdObject*> objects; // Scene lights
        std::list<Light*> lights;      // Scene lights
        Color bgColor;

    public:
        RenderContext(float step
                     ,const std::list<StdObject*>& objects
                     ,const std::list<Light*>& lights
                     ,const Color& bgColor)
        { 
            this->step    = step;
            this->objects = objects;
            this->lights  = lights;
            this->bgColor = bgColor;
            this->interpolate = false;
        };

        float getStep() const { return this->step; }
        const std::list<StdObject*>& getObjects() const { return this->objects; } 
        const std::list<Light*>& getLights() const      { return this->lights; } 
        const Color& getBackground() const           { return this->bgColor; }
        bool getInterpolation() const                   { return this->interpolate; }
        void setInterpolation(bool interpolate) { this->interpolate = interpolate; }
}; 

#endif
