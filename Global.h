//==============================================================================
/*
Filename:	Global.h
Project:	Haptics Showroom
Authors:	Naina Dhingra, Ke Xu, Hannes Bohnengel
Revision:	0.2
Remarks:	These files are tracked with git and are available on the github
repository: https://github.com/hannesb0/haptics-showroom
*/
//==============================================================================

#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "chai3d.h"

using namespace chai3d;
using namespace std;

//------------------------------------------------------------------------------
// GLOBAL CONSTANTS
//------------------------------------------------------------------------------

// default filename of image for graphical display 
const string defaultTextureImage = "brick-color.png";

// default filename of normal map for haptics display 
const string defaultNormalImage = "brick-normal.png";

// default filename of audio file (for voice coil)
const string defaultAudio = "audio.wav";

// default temperature
const int defaultTemperature = 3;

// default stiffness
const double defaultStiffness = 0.7f;

// default staticFriction
const double defaultStaticFriction = 0.2;

// default dynamicFriction
const double defaultDynamicFriction = 0.2;

// default texture level
const double defaultTextureLevel = 0.1;

// set default audio friction gain
const double defaultAudioGain = 0;

// set default audio pitch
const double defaultAudioPitch = 0.2;

//------------------------------------------------------------------------------
// ENUMERATIONS
//------------------------------------------------------------------------------

enum MyShape { cube, sphere, cylinder, complex3ds };

//------------------------------------------------------------------------------
// DECLARED MACROS
//------------------------------------------------------------------------------

// convert to resource path
#define RESOURCE_PATH(p)    (char*)((resourcesPath+string(p)).c_str())

// concatenates string q and p (e.g. q = "abc", p = "xyz" -> STR_ADD(q,p) = "abcxyz")
#define STR_ADD(q,p)		(char*)((string(q)+string(p)).c_str())

// initial position
#define INITIAL_POSITION	cVector3d(2.0, 0.0, 0.4)


#endif