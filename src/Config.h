#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

#include <memory>
#include <iostream>
#include <list>
#include <string>
#include <glm/glm.hpp>
#include "Voxel.h"

/******************************************************************************/

using namespace std;
using namespace glm;

/******************************************************************************/

typedef struct PositionDecl 
{
    float x,y,z;

    PositionDecl() : x(0.0f), y(0.0f), z(0.0f) { };
    PositionDecl(float _x, float _y, float _z) : x(_x), y(_y), z(_z) { };

} PositionDecl;

typedef struct ColorDecl 
{
    float r,g,b;

    ColorDecl() : r(0.0f), g(0.0f), b(0.0f) { };
    ColorDecl(float _r, float _g, float _b) : r(_r), g(_g), b(_b) { };

} ColorDecl;

/*******************************************************************************
 * Base configuration file format
 ******************************************************************************/

class Configuration
{
    protected:
        /**
         * All objects defined
         */
        list<Primitive*> objects;

        /**
         * All light instances encountered
         */
        list<Light*> lights;

        bool isNonNumeric(const string& s) const;
        virtual string readAttribute(string optionType, istream& is);
        virtual void readHeader(istream& s);
        virtual void readBody(istream& s, bool skippedHeader) = 0;
        virtual void addLighting();

    public:
        /**
         * The step size to use in your rendering, a float
         */
        float STEP;

        /** 
         * The number of voxels wide, tall, and deep the buffer measures
         */
        ivec3 XYZC;

        /**
         * The background color in floating point format
         */
        fvec3 BRGB;

        /** 
         * The box’s (voxel buffer’s) material RGB value in floating 
         * point format
         */
        fvec3 MRGB;

        /** 
         * The filename to give the output image
         */
        string FILE;

        /**
         * Two integers specifying the width and height (in pixels) of 
         * he ray casting that should be performed
         */
        ivec2 RESO;

        /** 
         * The x, y, z position of the eye/camera in world-space
         */
        fvec3 EYEP;

        /**
         * The viewing direction of the eye/camera towards the box, in 
         * world-space
         */
        fvec3 VDIR;

        /**
         * The up-vector in world-space
         */
        fvec3 UVEC;

        /**
         * The half-angle field of view in the Y-direction in degrees
         */
        float FOVY;

        /**
         * The position of the light in world-space
         */
        list<PositionDecl> LPOS;

        /** 
         * The color of the point light, in floating point format
         */
        list<ColorDecl> LCOL;

        /** 
         * Global random seed value
         */
        int SEED;

        Configuration();
        Configuration(const Configuration& other);
        virtual ~Configuration();

        virtual void read(istream& s, bool skipHeader = false);

        const list<Primitive*>& getObjects() const { return this->objects; };
        const list<Light*>& getLights() const      { return this->lights; };

        friend ostream& operator<<(ostream& s, const Configuration& c);
};

/******************************************************************************/

class NewConfigurationReader : public Configuration
{
    protected:
        void readObjectType(const string& line, string& objectType);
        void readCenter(const string& line, float& xCenter, float& yCenter, float& zCenter);
        void readRadius(const string& line, float& radius);
        void readAdditional(const string& line
                           ,float& scale
                           ,int& octaves
                           ,float& freq
                           ,float& amp
                           ,string& textureFile);
        virtual void readBody(istream& s, bool skippedHeader);

    public:
        NewConfigurationReader();
};

/******************************************************************************/

class OldConfigurationReader : public Configuration
{
    protected:
        float readSingleDensity(int count, string line);
        virtual void readBody(istream& s, bool skippedHeader);

    public:
        OldConfigurationReader();
};

/******************************************************************************/

#endif
