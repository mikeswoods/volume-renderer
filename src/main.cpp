#include <cstdlib>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <glm/glm.hpp>
#include <optionparser.h>
#include <CImg.h>
#include "R3.h"
#include "Color.h"
#include "Camera.h"
#include "StdObject.h"
#include "Light.h"
#include "Box.h"
#include "Config.h"
#include "Context.h"
#include "Voxel.h"

/******************************************************************************/

using namespace std;
using namespace cimg_library;
using namespace glm;

/******************************************************************************/

#ifdef None
#undef None
#endif

/*******************************************************************************
 * Command line options
 ******************************************************************************/

enum OptionIndex { 
  UNKNOWN
  ,HELP
  ,STEP_SIZE
  ,GRID_WIDTH
  ,GRID_HEIGHT
  ,GRID_DEPTH
  ,STEP
};

const option::Descriptor usage[] =
{
  {
     UNKNOWN
    ,0
    ,"" 
    ,""
    ,option::Arg::None
    ,"USAGE: ./smokeSimulation [options]\n\n Options:" 
  },
  {
     HELP
    ,0
    ,""
    ,"help"
    ,option::Arg::None
    ,"  --help  \t\tPrint usage and exit."
  },
  {
     STEP_SIZE
    ,0
    ,"W"
    ,"width"
    ,option::Arg::Optional
    ,"  -S/--step \t\tStep size (float)"
  },
  {
     GRID_WIDTH
    ,0
    ,"W"
    ,"width"
    ,option::Arg::Optional
    ,"  -W/--width \t\tGrid width (X), in cells"
  },
  {
     GRID_HEIGHT
    ,0
    ,"H"
    ,"height"
    ,option::Arg::Optional
    ,"  -H/--height \t\tGrid height (Y), in cells"
  },
  {
     GRID_DEPTH
    ,0
    ,"D"
    ,"depth"
    ,option::Arg::Optional
    ,"  -D/--depth \t\tGrid depth (Z), in cells"
  },
 
  {
     UNKNOWN
    ,0
    ,""
    ,""
    ,option::Arg::None
    ,"\nExamples:\n"
     "  smokeSimulation --unknown -- --this_is_no_option\n"
     "  smokeSimulation -unk --plus -ppp file1 file2\n" 
  },
  {0, 0, 0, 0, 0, 0}
};

/******************************************************************************/

Configuration readConfig(string filename)
{
	ifstream configFile(filename.c_str());
	Configuration config;
	config.read(configFile);
	configFile.close();
	return config;
}

void intializeScene(const Configuration& config
	               ,float& step
	               ,Camera& camera
	               ,Color& bgColor
	               ,string& filename
	               ,ivec2& resolution)
{
	step = config.STEP;

	// Camera
	camera.setPosition(P(config.EYEP[0],config.EYEP[1],config.EYEP[2]));
	camera.setViewDir(V(config.VDIR[0],config.VDIR[1],config.VDIR[2]));
	camera.setUp(V(config.UVEC[0],config.UVEC[1],config.UVEC[2]));
	camera.setFOV(config.FOVY);

	// Materials
	bgColor = Color(config.BRGB[0],config.BRGB[1],config.BRGB[2]);

	filename   = config.FILE; 
    resolution = config.RESO;

	camera.setAspectRatio(static_cast<float>(config.RESO.x) / static_cast<float>(config.RESO.y));
}

static void printUsageAndExit(const char* name)
{
	cerr << "Usage: " << name << ": <scene-file>" << endl;
	exit(1);
}

/******************************************************************************/

void render(CImg<unsigned char>& output
	       ,int resX
	       ,int resY
	       ,const Camera& camera
	       ,const RenderContext& context)
{
	list<StdObject*> objects = context.getObjects();

	if (context.getInterpolation()) {
		cout << "*** USING TRILINEAR INTERPOLATION ***" << endl;
	}	

	for(int i=0; i<resX; i++) {

		for (int j=0; j<resY; j++) {

			Ray ray = camera.spawnRay(i, j, resX, resY);
			Hit hit;

			Color accumColor(0,0,0);
			float accumTransmittance = 1.0f;

			for (auto oi = objects.begin(); oi != objects.end(); oi++) {

				if ((*oi)->intersects(ray, context, hit)) {
					accumColor += hit.color;
					accumTransmittance *= hit.transmittance;
				}
			}

			// Set the pixel color:
			Color screenPixel  = accumColor + (context.getBackground() * accumTransmittance);

			output(i, j, 0, 0) = static_cast<unsigned char>(screenPixel.iR());
			output(i, j, 0, 1) = static_cast<unsigned char>(screenPixel.iG());
			output(i, j, 0, 2) = static_cast<unsigned char>(screenPixel.iB());
		}

		clog << "Rendering line: " << i << "\r";
	}	

	clog << endl << "Done!" << endl;
}

/******************************************************************************/

int main(int argc, char** argv) 
{
	if (argc < 2) {
		printUsageAndExit(argv[0]);
	}

	float step;
	Camera camera;
	Color bgColor, matColor;
	string filename;
	ivec2 resolution;
	bool doInterpolation = false;

	if (argc >= 3 && string(argv[2]) == "interpolate") {
		doInterpolation = true;
	}	

	Configuration config = readConfig(argv[1]);

	intializeScene(config, step, camera, bgColor, filename, resolution);

	cout << config << endl
	     << camera << endl;

	CImg<unsigned char> output(resolution.x, resolution.y, 1, 3, 0);
	RenderContext context(step, config.getObjects(), config.getLights(), bgColor);
	context.setInterpolation(doInterpolation);

	render(output, resolution.x, resolution.y, camera, context);

	output.save(filename.c_str());

	return 0;
}

/******************************************************************************/
