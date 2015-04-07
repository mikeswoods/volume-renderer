cmake_minimum_required (VERSION 3.0.0)

################################################################################
# To enable OpenMP support in the simulator, uncomment the line below.
# Note: if compiling on OSX (10.8.x Mavericks as I write this), be aware that
# clang/clang++ does not support OpenMP out of the box. You must have g++
# installed and it must be in your current path.
################################################################################

set (ENABLE_OPENMP 1)

################################################################################

# Only use g++ if we're using OpenMP:
if (DEFINED ENABLE_OPENMP)
   MESSAGE("-- Enabled OpenMP")
   set (CMAKE_C_COMPILER gcc)
   set (CMAKE_CXX_COMPILER g++)
   # Let the preprocessor know we're using OpenMP too:
   add_definitions(-DENABLE_OPENMP)
else()
   # Otherwise, explicitly use clang:
   set (CMAKE_C_COMPILER clang)
   set (CMAKE_CXX_COMPILER clang++)
endif ()

project (VolumeRenderer)
list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external")

set (EXTERNAL "external")

# Set up include and lib paths
include_directories (${EXTERNAL}/include)

#OSX-specific hacks/fixes
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   	#Link IOKit because this is where we get GL stuff for OSX
   	set (IOKIT "-framework IOKit")
	set (CORELIBS ${CORELIBS} ${IOKIT})
endif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

# Added my own flags: warnings all the way:
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

#Compiler flag magic:
if (UNIX AND NOT APPLE)
   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -msse2")
elseif (APPLE)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -m64 -msse2 -ggdb -I/opt/X11/include")
elseif (WIN32)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif ()

# Remember to add the "-fopenmp" flag when compiling also:
if (DEFINED ENABLE_OPENMP)
   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif ()

# Linker flags:
if (UNIX AND NOT APPLE)
   set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lX11 -lpthread")
elseif (APPLE)
   set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/opt/X11/lib -lX11")
endif()

# Add all source files. Headers don't need to be listed here since the compiler will find them;
# we just need the actual files being fed directly to the compiler
set (SOURCE_FILES "src/BV.cpp"
                  "src/BitmapTexture.cpp"
                  "src/Camera.cpp"
                  "src/Color.cpp"
                  "src/Config.cpp"
                  "src/Light.cpp"
                  "src/Primitive.cpp"
                  "src/R3.cpp"
                  "src/Ray.cpp"
                  "src/Utils.cpp"
                  "src/Voxel.cpp"
                  "src/VoxelCloud.cpp"
                  "src/VoxelPyroclastic.cpp"
                  "src/VoxelSphere.cpp"
                  "src/main.cpp"
                  "src/perlin.cpp")

add_executable(VolumeRenderer ${SOURCE_FILES})

target_link_libraries (VolumeRenderer ${CORELIBS})
