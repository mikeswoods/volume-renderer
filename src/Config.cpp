#include <cctype>
#include <cassert>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <ctime>
#include "Utils.h"
#include "BitmapTexture.h"
#include "Config.h"
#include "Light.h"
#include "VoxelSphere.h"
#include "VoxelCloud.h"
#include "VoxelPyroclastic.h"

#define DEFAULT_OCTAVES 1
#define DEFAULT_FREQ 1.0f
#define DEFAULT_AMP 1.0f

/******************************************************************************/
 
using namespace std;
using namespace Utils;

/*******************************************************************************
 * ==== New format ====
 * 
 * STEP 0.005
 * XYZC 100 100 100
 * BRGB 0.27 0.51 0.71
 * MRGB 0.96 0.96 0.96
 * FILE BabysFirstCloudImage2.bmp
 * RESO 640 480
 * EYEP 0 0 2
 * VDIR 0 0 -1
 * UVEC 0 1 0
 * FOVY 45
 * LPOS 2 0 0
 * LCOL 1 1 1
 * 
 * 1
 * 
 * sphere
 * 0.5 0.5 -0.5
 * 0.4
 *
 * ==== Old format ====
 *
 * STEP 0.005
 * XYZC 100 100 100
 * BRGB 0.27 0.51 0.71
 * MRGB 0.96 0.96 0.96
 * FILE BabysFirstCloudImage2.bmp
 * RESO 640 480
 * EYEP 0 0 2
 * VDIR 0 0 -1
 * UVEC 0 1 0
 * FOVY 45
 * LPOS 2 0 0
 * LCOL 1 1 1
 * 
 * 0
 * 0
 * 0
 * 0.75
 * ...
 * 0.75
 * 1.0
 * 0
 * 0
 ******************************************************************************/

Configuration::Configuration() :
    STEP(0.005),
    XYZC(ivec3(0, 0, 0)),
    BRGB(ivec3(0, 0, 0)),
    MRGB(ivec3(0, 0, 0)),
    FILE(""),
    RESO(ivec2(640, 480)),
    EYEP(ivec3(0, 0, 2)),
    VDIR(ivec3(0, 0, -1)),
    UVEC(ivec3(0, 1, 0)),
    FOVY(45.0f),
    SEED(static_cast<int>(time(nullptr)))
{

}

Configuration::Configuration(const Configuration& other) :
    STEP(other.STEP),
    XYZC(other.XYZC),
    BRGB(other.BRGB),
    MRGB(other.MRGB),
    FILE(other.FILE),
    RESO(other.RESO),
    EYEP(other.EYEP),
    VDIR(other.VDIR),
    UVEC(other.UVEC),
    FOVY(other.FOVY),
    SEED(static_cast<int>(time(nullptr)))
{

}

Configuration::~Configuration()
{

}

ostream& operator<<(ostream& s, const Configuration& c)
{
    return
    s << "Configuration {"  << endl
      << "  STEP      = "   << c.STEP << endl
      << "  XYZC      = <"  << c.XYZC.x << "," << c.XYZC.y << "," << c.XYZC.z << ">" << endl
      << "  BRGB      = <"  << c.BRGB.r << "," << c.BRGB.g << "," << c.BRGB.b << ">" << endl
      << "  MRGB      = <"  << c.MRGB.r << "," << c.MRGB.g << "," << c.MRGB.b << ">" << endl
      << "  FILE      = \"" << c.FILE << "\"" << endl
      << "  RESO      = <"  << c.RESO.x << "," << c.RESO.y << ">" << endl
      << "  EYEP      = <"  << c.EYEP.x << "," << c.EYEP.y << "," << c.EYEP.z << ">" << endl
      << "  VDIR      = <"  << c.VDIR.x << "," << c.VDIR.y << "," << c.VDIR.z << ">" << endl
      << "  UVEC      = <"  << c.UVEC.x << "," << c.UVEC.y << "," << c.UVEC.z << ">" << endl
      << "  FOVY      = "   << c.FOVY << endl
      << "  SEED      = "   << c.SEED << endl
      << "  |lights|  = "   << c.lights.size() << endl
      << "  |objects| = "   << c.objects.size() << endl
      << "}";
}

/**
 * Tests if the given string is non-numeric
 */
bool Configuration::isNonNumeric(const string& s) const
{
    string::const_iterator it = s.begin();
    
    while (it != s.end() && (isalpha(*it) || isspace(*it))) {
        ++it;
    }

    return !s.empty() && it == s.end();
}

/**
 * Reads a configuration header attribute from the given stream
 */
string Configuration::readAttribute(string optionType, istream& is)
{
    if (optionType == "STEP") {
        is >> this->STEP;
    } else if (optionType == "XYZC") {
        is >> this->XYZC.x >> this->XYZC.y >> this->XYZC.z;
    } else if (optionType == "BRGB") {
        is >> this->BRGB.r >> this->BRGB.g >> this->BRGB.b;
    } else if (optionType == "MRGB") {
        is >> this->MRGB.r >> this->MRGB.g >> this->MRGB.b;
    } else if (optionType == "FILE") {
        is >> this->FILE;
    } else if (optionType == "RESO") {
        is >> this->RESO.x >> this->RESO.y;
    } else if (optionType == "EYEP") {
        is >> this->EYEP.x >> this->EYEP.y >> this->EYEP.z;
    } else if (optionType == "VDIR") {
        is >> this->VDIR.x >> this->VDIR.y >> this->VDIR.z;
    } else if (optionType == "UVEC") {
        is >> this->UVEC.x >> this->UVEC.y >> this->UVEC.z;
    } else if (optionType == "FOVY") {
        is >> this->FOVY;
    } else if (optionType == "SEED") {
        is >> this->SEED;
    } else if (optionType == "LPOS") {
        PositionDecl position;
        is >> position.x >> position.y >> position.z;
        this->LPOS.push_back(position);
    } else if (optionType == "LCOL") {
        ColorDecl color;
        is >> color.r >> color.g >> color.b;
        this->LCOL.push_back(color);
    } else {
        throw runtime_error("Invalid option type: " + optionType);
    }

    return optionType;
}

/**
 * Reads a configuration header from the given stream, updating the 
 * corresponding member values of this instance
 */
void Configuration::readHeader(istream& is)
{
    string line, optionType;
    long rewindChars = 0;

    cout.flush();

    while (getline(is, line)) {

        line = trim(line);

        // Newline means the header is over
        if (line.length() == 0) {
            break;
        }

        istringstream is(line);

        is >> optionType;

        // Similarly, if optionType is read as a number, then the header is 
        // considered over as well
        if (!this->isNonNumeric(optionType)) {
            break;
        }

        this->readAttribute(optionType, is);
    }

    rewindChars = line.length() + 1;
    is.seekg(-rewindChars, ios_base::cur);
}

/**
 * Instantiates lights based on read configuration values
 */
void Configuration::addLighting()
{
    if (this->LPOS.size() > 0 &&
        this->LCOL.size() > 0 &&
        this->LPOS.size() != this->LCOL.size()) 
    {
        cerr << "***BAD*** "
             << "this->LPOS.size() = " << this->LPOS.size() << " ; "
             << "this->LCOL.size() = " << this->LCOL.size()
             << endl;
        throw length_error("POS.size() != LCOL.size()");
    }

    if (this->LPOS.size() > MAX_LIGHTS) {
        throw length_error("#lights > MAX_LIGHTS");
    }

    auto ip = this->LPOS.begin();
    auto ic = this->LCOL.begin();

    for (ip = this->LPOS.begin(), ic = this->LCOL.begin()
        ;ip != this->LPOS.end() && ic != this->LCOL.end()
        ;ip++, ic++) 
    {
        Light *light = new Light(P(ip->x, ip->y, ip->z)
                                ,Color(ic->r,ic->g,ic->b)); 
        lights.push_back(light);
    }
}

/** 
 * Reads a configuration from the given stream, updating the 
 * corresponding member values of this instance
 */
void Configuration::read(istream& is, bool skipHeader)
{
    if (skipHeader) {
        clog << "Skipping header..." << endl;
    } else {
        this->readHeader(is);
    }

    this->readBody(is, skipHeader);

    if (skipHeader) {

    } else {
        // Let there be light(s):
        this->addLighting();
    }
}

/******************************************************************************/

NewConfigurationReader::NewConfigurationReader() :
    Configuration()
{

}

/**
 * Reads a line that specifies an objects type: "sphere", "proclastic", etc.
 */
void NewConfigurationReader::readObjectType(const string& line, string& objectType)
{
    istringstream is(line);

    is >> objectType;

    if (is.fail()) {
        cerr << "objectType = "<<objectType<<endl;
        throw runtime_error("bad parse in <readObjectType>: " + line);
    }
}

/** 
 * Reads a line that contains an objects <xCenter> <yCenter> <zCenter> values
 */
void NewConfigurationReader::readCenter(const string& line, float& xCenter, float& yCenter, float& zCenter)
{
    istringstream is(line);

    is >> xCenter >> yCenter >> zCenter;

    if (is.fail()) {
        cerr << "xCenter = "<<xCenter<<", yCenter = "<<yCenter<<", zCenter = "<<zCenter<<endl;
        throw runtime_error("bad parse in <readCenter>: " + line);
    }
}

/** 
 * Reads a line that contains an objects <radius> value
 */
void NewConfigurationReader::readRadius(const string& line, float& radius)
{
    istringstream is(line);

    is >> radius;

    if (is.fail()) {
        throw runtime_error("bad parse in <readRadius>: " + line);
    }
}

/**
 * Reads the remaining additional attributes, if any
 */
void NewConfigurationReader::readAdditional(const string& line
                                           ,float& scale
                                           ,int& octaves
                                           ,float& freq
                                           ,float& amp
                                           ,string& textureFile)
{
    istringstream is(line);

    is >> scale;
    if (is.fail()) {
        return;
    }

    is >> octaves;
    if (is.fail()) {
        return;
    }

    is >> freq;
    if (is.fail()) {
        return;
    }

    is >> amp;
    if (is.fail()) {
        return;
    }

    is >> textureFile;
    if (is.fail()) {
        return;
    }
}

/**
 * Read the new version of body
 */
void NewConfigurationReader::readBody(istream& is, bool skippedHeader)
{
    string line;
    int objectCount = 0;

    is >> objectCount;

    if (!is.good()) {
        throw runtime_error("body: Couldn't parse object count");
    }

    // Expect a blank line:
    getline(is, line);

    line = trim(line);
    if (line.length() > 0) {
        throw runtime_error("Expected one or more blank line after object count");
    }

    getline(is, line);

    // Now using object count, read each object definition:
    for (int i=0; i<objectCount; i++) {

        string objectType;

        // Sane default values for center, radius, and scale
        float xCenter = 0.0f, yCenter = 0.0f, zCenter = 0.0f;
        float radius  = 0.0f;
        float scale   = this->XYZC.x;
        int octaves   = 1;
        float freq    = 1.0f;
        float amp     = 1.0f;
        string textureFile = "";

        // Read the next three lines and expect the following format:
        // for defining the attributes of an object
        // - Line 1: "<objectType>"
        // - Line 2: "<xCenter> <yCenter> <zCenter>"
        // - Line 3: "<radius>"
        // - Line 4: either blank or "<scale> <octaves> <freq> <amp> <texture>"
        for (int j=0; j<4; j++) {

            getline(is, line);
            line = trim(line);
            clog << "read["<<line.length()<<"]("<<i<<","<<j<<"): "<<line<<endl;

            switch (j) {
                case 0:
                    clog<<"parse("<<i<<","<<j<<"): <readObjectType>"<<endl;
                    this->readObjectType(line, objectType);
                    break;
                case 1:
                    clog<<"parse("<<i<<","<<j<<"): <readCenter>"<<endl;
                    this->readCenter(line, xCenter, yCenter, zCenter);
                    break;
                case 2:
                    clog<<"parse("<<i<<","<<j<<"): <readRadius>"<<endl;
                    this->readRadius(line, radius);
                    break;
                case 3:
                    clog<<"parse("<<i<<","<<j<<"): <readAdditional>"<<endl;
                    
                    if (line.length() > 0 && !is.eof()) {
                        this->readAdditional(line, scale, octaves, freq, amp, textureFile);
                        // Read and skip the next empty line:
                        getline(is, line); 
                    }
                    break;
            }
        }

        Primitive* obj     = nullptr;
        shared_ptr<Material> material(nullptr); 

        if (textureFile != "") {
            material = make_shared<BitmapTexture>(textureFile);
        } else {
            material = make_shared<Color>(this->MRGB.r, this->MRGB.g, this->MRGB.b);
        }

        // Define the bounds of the object as a function of the radius and 
        // center provided
        BoundingBox bounds = BoundingBox::fromCenter(P(xCenter,yCenter,zCenter)
                                                      ,radius);

        if  (objectType == "sphere") {

            obj = new VoxelSphere(radius, scale, this->XYZC, bounds, material);

        } else if (objectType == "cloud") {

            obj = new VoxelCloud(radius
                                ,scale
                                ,this->XYZC
                                ,bounds
                                ,material
                                ,this->SEED
                                ,octaves
                                ,freq
                                ,amp);

        } else if (objectType == "pyroclastic") {

            obj = new VoxelPyroclastic(radius
                                      ,scale
                                      ,this->XYZC
                                      ,bounds
                                      ,material
                                      ,this->SEED
                                      ,octaves
                                      ,freq
                                      ,amp);
        }

        this->objects.push_back(obj);
    }
}

/******************************************************************************/

OldConfigurationReader::OldConfigurationReader() :
    Configuration()
{

}

/**
 * Reads a configuration density value for a single object
 */
float OldConfigurationReader::readSingleDensity(int count, string line)
{
    istringstream is(line);
    float value = 0.0;
    is >> value;

    if (!is.good() && !is.eof()) {
        ostringstream msg;
        msg << "(" << count << ") Bad parse for line: '" << line << "'";
        throw runtime_error(msg.str());
    }

    // Multiply the density value by the voxel space width:
    return value * this->XYZC.x;
}

/**
 * Read the old version of body. The configuration may contain a header 
 * specifying various rendering parameters such as grid width/height/depth,
 * ray marching step size, etc. or it may be just density data, with one
 * value per line
 */
void OldConfigurationReader::readBody(istream& is, bool skippedHeader)
{
    int count = 0;
    string line;

    BoundingBox bounds(P(0,0,0), P(1,1,-1));
    shared_ptr<Material> material = make_shared<Color>(this->MRGB.r, this->MRGB.g, this->MRGB.b);

    auto voxels = make_shared<vector<Voxel> >();
    voxels->reserve(1024 * 1024);

    while (getline(is, line)) {

        // Assume each line contains a floating point density value:
        line = trim(line);
        float density = this->readSingleDensity(count++, line);

        voxels->push_back(Voxel(density));
    }

    auto vb = new VoxelBuffer(voxels, bounds, material);

    if (skippedHeader) {
        // Defer explicit dimensions for now
    } else {
        assert(this->XYZC.x > 0 && this->XYZC.y > 0 && this->XYZC.z > 0);
        vb->setDimensions(this->XYZC);
    }

    this->objects.push_back(vb);
}

/******************************************************************************/
