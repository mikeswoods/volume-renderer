#include <cstdlib>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <optionparser.h>
#include <CImg.h>
#include "R3.h"
#include "Utils.h"
#include "Color.h"
#include "Camera.h"
#include "Primitive.h"
#include "Light.h"
#include "Config.h"
#include "Context.h"
#include "Voxel.h"

/******************************************************************************/

using namespace std;
using namespace cimg_library;
using namespace glm;
using namespace Utils;

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
  ,OUTPUT_FILENAME
  ,NO_INPUT_HEADER
  ,TRILINEAR_INTERPOLATION
};

const option::Descriptor usage[] =
{
  {
     UNKNOWN
    ,0
    ,"" 
    ,""
    ,option::Arg::None
    ,"USAGE: VolumeRenderer [options] <input-file>\n\n Options:" 
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
    ,"S"
    ,"step"
    ,option::Arg::Optional
    ,"  -S/--step \t\tStep size (float)"
  },
  {
     GRID_WIDTH
    ,0
    ,"W"
    ,"width"
    ,option::Arg::Optional
    ,"  -W/--width \t\tGrid width (X), in cells (int)"
  },
  {
     GRID_HEIGHT
    ,0
    ,"H"
    ,"height"
    ,option::Arg::Optional
    ,"  -H/--height \t\tGrid height (Y), in cells (int)"
  },
  {
     GRID_DEPTH
    ,0
    ,"D"
    ,"depth"
    ,option::Arg::Optional
    ,"  -D/--depth \t\tGrid depth (Z), in cells (int)"
  },
  {
     OUTPUT_FILENAME
    ,0
    ,"o"
    ,"output"
    ,option::Arg::Optional
    ,"  -o/--output \t\tOutput filename (string)"
  },
  {
     NO_INPUT_HEADER
    ,0
    ,"N"
    ,"no-header"
    ,option::Arg::None
    ,"  -N/--no-header \t\tSpecifies that the input file does not contain a header. If provided, -o/--output must be specified as well"
  },
  {
     TRILINEAR_INTERPOLATION
    ,0
    ,"I"
    ,"interpolation"
    ,option::Arg::None
    ,"  -I/--interpolation \t\tEnable trilinear interpolation"
  },
  {
     UNKNOWN
    ,0
    ,""
    ,""
    ,option::Arg::None
    ,""
  },
  {0, 0, 0, 0, 0, 0}
};

/******************************************************************************/


/******************************************************************************/

static shared_ptr<Configuration> readConfig(string filename
	                                       ,int version = 1
	                                       ,bool skipHeader = false)
{
	ifstream configFile(filename.c_str());
	shared_ptr<Configuration> config(nullptr);

	switch (version) {
		case 2:
			{
				config = make_shared<NewConfigurationReader>();
			}
			break;
		case 1:
		default:
			{
				config = make_shared<OldConfigurationReader>();
			}
			break;
	}

	config->read(configFile, skipHeader);
	configFile.close();

	return config;
}

/**
 *
 */
void intializeCamera(shared_ptr<Configuration> config, Camera& camera)
{
	camera.setPosition(P(config->EYEP.x, config->EYEP.y, config->EYEP.z));
	camera.setViewDir(V(config->VDIR.x, config->VDIR.y, config->VDIR.z));
	camera.setUp(V(config->UVEC.x, config->UVEC.y, config->UVEC.z));
	camera.setFOV(config->FOVY);
	camera.setAspectRatio(static_cast<float>(config->RESO.x) / static_cast<float>(config->RESO.y));
}

/******************************************************************************/

void render(CImg<unsigned char>& output
	         ,ivec2 resolution
	         ,const Camera& camera
	         ,const RenderContext& context)
{
	auto objects = context.getObjects();

	if (context.getInterpolation()) {
		cout << "*** USING TRILINEAR INTERPOLATION ***" << endl;
	}	

  #ifdef ENABLE_OPENMP
  #pragma omp parallel for collapse(2)
  #endif
	for(int i=0; i<resolution.x; i++) {

		for (int j=0; j<resolution.y; j++) {

			Ray ray = camera.spawnRay(i, j, resolution.x, resolution.y);
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

		//clog << "Rendering line: " << i << "\r";
	}	

	clog << endl << "Done!" << endl;
}

/******************************************************************************/

static void updateConfiguration(shared_ptr<Configuration> config
                               ,option::Option* options
                               ,bool skipHeader = false)
{
    bool success = false;

    // Step size
    if (options[STEP_SIZE].count() > 0 && options[STEP_SIZE].first()->arg != nullptr) {
        float stepSize = toNumber<float>(options[STEP_SIZE].first()->arg, success);
        config->STEP   = stepSize;
    }

    // Voxel grid width
    if (options[GRID_WIDTH].count() > 0 && options[GRID_WIDTH].first()->arg != nullptr) {
        int width  = toNumber<int>(options[GRID_WIDTH].first()->arg, success);
        if (success) {
            config->XYZC.x = width;
        }
    }

    // Voxel grid height
    if (options[GRID_HEIGHT].count() > 0 && options[GRID_HEIGHT].first()->arg != nullptr) {
        int height  = toNumber<int>(options[GRID_HEIGHT].first()->arg, success);
        if (success) {
            config->XYZC.y = height;
        }
    }

    // Voxel grid depth
    if (options[GRID_HEIGHT].count() > 0 && options[GRID_HEIGHT].first()->arg != nullptr) {
        int depth  = toNumber<int>(options[GRID_HEIGHT].first()->arg, success);
        if (success) {
            config->XYZC.z = depth;
        }
    }

    // Output file name
    if (options[OUTPUT_FILENAME].count() > 0 && options[OUTPUT_FILENAME].first()->arg != nullptr) {
       config->FILE  = string(options[OUTPUT_FILENAME].first()->arg);
    }

    assert(config->STEP > 0.0f);
    assert(config->XYZC.x > 0);
    assert(config->XYZC.y > 0);
    assert(config->XYZC.z > 0);
    assert(config->FILE != "");
    assert(config->RESO.x > 0 && config->RESO.y > 0);

    if (skipHeader) {
      cout << "Setting default background color" << endl;
      config->BRGB = fvec3(1.0f, 0.0f, 1.0f);
    }

    // Finally, for all of the objects that don't have set grid dimensions,
    // set them according to the arguments passed to the program:
    auto objects = config->getObjects();
    for (auto i = objects.begin(); i != objects.end(); i++) {
      if (!(*i)->hasLoadedDimensions()) {
        (*i)->setDimensions(config->XYZC);
      }
    }
}

/******************************************************************************/

int main(int argc, char** argv) 
{
  // Skip program name argv[0] if present:
  argc -= argc > 0;
  argv += argc > 0;

  option::Stats  stats(usage, argc, argv);
  option::Option options[64], buffer[4096];
  option::Parser parse(usage, argc, argv, options, buffer);

  if (parse.error()) {
      exit(EXIT_FAILURE);
  }

  if (argc < 1 || options[HELP]) {
      option::printUsage(std::cout, usage);
      exit(EXIT_SUCCESS);
  }

  // Are header options present in the input?
  bool noHeader = options[NO_INPUT_HEADER].count() > 0;
	Camera camera;

	auto config = readConfig(argv[argc-1], 1, noHeader);

  // Merge in and override what's in the configuration with options from
  // the command line:
  updateConfiguration(config, options);

	intializeCamera(config, camera);

  // Dump all the values used in the render:
  cout << *config << endl;
	cout << camera << endl;

  // What we'll write to:
	CImg<unsigned char> output(config->RESO.x, config->RESO.y, 1, 3, 0);

	RenderContext context(config->STEP, config->getObjects(), config->getLights(), Color(config->BRGB));

	context.setInterpolation(true);

	render(output, config->RESO, camera, context);

	output.save(config->FILE.c_str());

	return 0;
}

/******************************************************************************/
