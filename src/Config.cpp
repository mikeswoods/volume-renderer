#include <cctype>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <ctime>
#include "Utils.h"
#include "BitmapTexture.h"
#include "Config.h"
#include "Light.h"
#include "Box.h"
#include "VoxelSphere.h"
#include "VoxelCloud.h"
#include "VoxelPyroclastic.h"

#define DEFAULT_OCTAVES 1
#define DEFAULT_FREQ 1.0f
#define DEFAULT_AMP 1.0f

////////////////////////////////////////////////////////////////////////////////
// Configuration file format
////////////////////////////////////////////////////////////////////////////////

// New format

/**
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
 */

////////////////////////////////////////////////////////////////////////////////

// Old format

/**
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
 */
using namespace std;

Configuration::Configuration()
{
    this->STEP = 0.0f;

    this->XYZC[0] = 0;
    this->XYZC[1] = 0;
    this->XYZC[2] = 0;

    this->BRGB[0] = 0.0f;
    this->BRGB[1] = 0.0f;
    this->BRGB[2] = 0.0f;

    this->MRGB[0] = 0.0f;
    this->MRGB[1] = 0.0f;
    this->MRGB[2] = 0.0f;

    this->FILE = "";

    this->RESO[0] = 0;
    this->RESO[1] = 0;

    this->EYEP[0] = 0.0f;
    this->EYEP[1] = 0.0f;
    this->EYEP[2] = 0.0f;

    this->VDIR[0] = 0.0f;
    this->VDIR[1] = 0.0f;
    this->VDIR[2] = 0.0f;

    this->UVEC[0] = 0.0f;
    this->UVEC[1] = 0.0f;
    this->UVEC[2] = 0.0f;

    this->FOVY = 0.0f;

    this->SEED = (int)time(NULL);
}

ostream& operator<<(ostream& s, const Configuration& c)
{
    return
    s << "Configuration {"  << endl
      << "  STEP      = "   << c.STEP << endl
      << "  XYZC      = <"  << c.XYZC[0] << "," << c.XYZC[1] << "," << c.XYZC[2] << ">" << endl
      << "  BRGB      = <"  << c.BRGB[0] << "," << c.BRGB[1] << "," << c.BRGB[2] << ">" << endl
      << "  MRGB      = <"  << c.MRGB[0] << "," << c.MRGB[1] << "," << c.MRGB[2] << ">" << endl
      << "  FILE      = \"" << c.FILE << "\"" << endl
      << "  RESO      = <"  << c.RESO[0] << "," << c.RESO[1] << ">" << endl
      << "  EYEP      = <"  << c.EYEP[0] << "," << c.EYEP[1] << "," << c.EYEP[2] << ">" << endl
      << "  VDIR      = <"  << c.VDIR[0] << "," << c.VDIR[1] << "," << c.VDIR[2] << ">" << endl
      << "  UVEC      = <"  << c.UVEC[0] << "," << c.UVEC[1] << "," << c.UVEC[2] << ">" << endl
      << "  FOVY      = "   << c.FOVY << endl
      << "  SEED      = "   << c.SEED << endl
      << "  |lights|  = "   << c.lights.size() << endl
      << "  |objects| = "   << c.objects.size() << endl
      << "}";
}

// Tests if the given string is non-numeric
bool Configuration::isNonNumeric(const string& s)
{
    string::const_iterator it = s.begin();
    
    while (it != s.end() && (isalpha(*it) || isspace(*it))) {
        ++it;
    }

    return !s.empty() && it == s.end();
}

// Reads a configuration header attribute from the given stream
string Configuration::readAttribute(string optionType, istream& is)
{
    if (optionType == "STEP") {
        is >> this->STEP;
    } else if (optionType == "XYZC") {
        is >> this->XYZC[0] >> this->XYZC[1] >> this->XYZC[2];
    } else if (optionType == "BRGB") {
        is >> this->BRGB[0] >> this->BRGB[1] >> this->BRGB[2];
    } else if (optionType == "MRGB") {
        is >> this->MRGB[0] >> this->MRGB[1] >> this->MRGB[2];
    } else if (optionType == "FILE") {
        is >> this->FILE;
    } else if (optionType == "RESO") {
        is >> this->RESO[0] >> this->RESO[1];
    } else if (optionType == "EYEP") {
        is >> this->EYEP[0] >> this->EYEP[1] >> this->EYEP[2];
    } else if (optionType == "VDIR") {
        is >> this->VDIR[0] >> this->VDIR[1] >> this->VDIR[2];
    } else if (optionType == "UVEC") {
        is >> this->UVEC[0] >> this->UVEC[1] >> this->UVEC[2];
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

// Reads a configuration density value for a single object
float Configuration::readSingleDensity(int count, string line)
{
    istringstream is(line);
    float value = 0.0;
    is >> value;

    if (!is.good() && !is.eof()) {
        ostringstream msg;
        msg << "(" << count << ") Bad parse for line: '" << line << "'";
        throw runtime_error(msg.str());
    }

    return value;
}

// Reads a configuration density value for a single object
void Configuration::readObjectDescription(istream& is)
{
    float value = 0.0;
    is >> value;
}

// Reads a configuration header from the given stream, updating the 
// corresponding member values of this instance
void Configuration::readHeader(istream& is)
{
    string line, optionType;
    long rewindChars = 0;

    cout.flush();

    while (getline(is, line)) {

        line = Utils::trim(line);

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

// Read the old version of body
void Configuration::readOldFormatBody(istream& is)
{
    int count = 0;
    int index = 0;
    string line;

    ////////////////////////////////////////////////////////////////////////////
    // Old
    ////////////////////////////////////////////////////////////////////////////

    Color *material = new Color(this->MRGB[0] ,this->MRGB[1] ,this->MRGB[2]);

    VoxelBuffer *vb = new VoxelBuffer(this->XYZC[0]
                                     ,this->XYZC[1]
                                     ,this->XYZC[2]
                                     ,BoundingBox(P(0,0,0),P(1,1,-1))
                                     ,material);

    while (getline(is, line)) {

        line = Utils::trim(line);

        // Multiply the density value by the voxel space width:
        vb->set(index++, Voxel(readSingleDensity(count++, line) * this->XYZC[0]));
    }

    this->objects.push_back(vb);
}

// Reads a line that specifies an objects type: "sphere", "proclastic", etc.
void Configuration::readObjectType(const string& line, string& objectType)
{
    istringstream is(line);

    is >> objectType;

    if (is.fail()) {
        cerr << "objectType = "<<objectType<<endl;
        throw runtime_error("bad parse in <readObjectType>: " + line);
    }
}

// Reads a line that contains an objects <xCenter> <yCenter> <zCenter> values
void Configuration::readCenter(const string& line, float& xCenter, float& yCenter, float& zCenter)
{
    istringstream is(line);

    is >> xCenter >> yCenter >> zCenter;

    if (is.fail()) {
        cerr << "xCenter = "<<xCenter<<", yCenter = "<<yCenter<<", zCenter = "<<zCenter<<endl;
        throw runtime_error("bad parse in <readCenter>: " + line);
    }
}

// Reads a line that contains an objects <radius> value
void Configuration::readRadius(const string& line, float& radius)
{
    istringstream is(line);

    is >> radius;

    if (is.fail()) {
        throw runtime_error("bad parse in <readRadius>: " + line);
    }
}

// Reads the remaining additional attributes, if any
void Configuration::readAdditional(const string& line
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

// Read the new version of body
void Configuration::readNewFormatBody(istream& is)
{
    string line;
    int objectCount = 0;

    is >> objectCount;

    if (!is.good()) {
        throw runtime_error("body: Couldn't parse object count");
    }

    // Expect a blank line:
    getline(is, line);
    line = Utils::trim(line);
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
        float scale   = this->XYZC[0];
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
            line = Utils::trim(line);
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

        StdObject* obj     = NULL;
        Material *material = NULL; 

        if (textureFile != "") {
            material = new BitmapTexture(textureFile);
        } else {
            material = new Color(this->MRGB[0] ,this->MRGB[1] ,this->MRGB[2]);
        }

        // Define the bounds of the object as a function of the radius and 
        // center provided
        BoundingBox bounds = BoundingBox::fromCenter(P(xCenter,yCenter,zCenter)
                                                      ,radius);

        if  (objectType == "sphere") {

            obj = new VoxelSphere(radius
                                 ,scale
                                 ,this->XYZC[0]
                                 ,this->XYZC[1]
                                 ,this->XYZC[2]
                                 ,bounds
                                 ,material);

        } else if (objectType == "cloud") {

            obj = new VoxelCloud(radius
                                ,scale
                                ,this->XYZC[0]
                                ,this->XYZC[1]
                                ,this->XYZC[2]
                                ,bounds
                                ,material
                                ,this->SEED
                                ,octaves
                                ,freq
                                ,amp);

        } else if (objectType == "pyroclastic") {

            obj = new VoxelPyroclastic(radius
                                      ,scale
                                      ,this->XYZC[0]
                                      ,this->XYZC[1]
                                      ,this->XYZC[2]
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

// Reads a configuration header from the given stream, updating the 
// corresponding member values of this instance
void Configuration::readBody(istream& is)
{  
    //this->readOldFormatBody(is);
    this->readNewFormatBody(is);
}

// Reads a configuration from the given stream, updating the 
// corresponding member values of this instance
void Configuration::read(istream& is)
{
    this->readHeader(is);

    this->readBody(is);

    // Let there be light(s):

    if (this->LPOS.size() != this->LCOL.size()) {
        cerr << "***BAD*** "
             << "this->LPOS.size() = " << this->LPOS.size() << " ; "
             << "this->LCOL.size() = " << this->LCOL.size()
             << endl;
        throw length_error("POS.size() != LCOL.size()");
    }

    if (this->LPOS.size() > MAX_LIGHTS) {
        throw length_error("#lights > MAX_LIGHTS");
    }

    list<PositionDecl>::const_iterator ip = this->LPOS.begin();
    list<ColorDecl>::const_iterator ic    = this->LCOL.begin();

    for (ip = this->LPOS.begin(), ic = this->LCOL.begin()
        ;ip != this->LPOS.end() && ic != this->LCOL.end()
        ;ip++, ic++) 
    {
        Light *light = new Light(P(ip->x, ip->y, ip->z)
                                ,Color(ic->r,ic->g,ic->b)); 
        lights.push_back(light);
    }
}

