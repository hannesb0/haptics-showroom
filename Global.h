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
// ENUMERATIONS
//------------------------------------------------------------------------------

enum MyShape { plane, cube, sphere, cylinder, complex3ds };
enum MyTempStatus { heating, cooling, standby };

//------------------------------------------------------------------------------
// STRUCTS
//------------------------------------------------------------------------------

struct MyOrientation{
	cVector3d	axis;
	double		rotation;
};

//------------------------------------------------------------------------------
// GLOBAL CONSTANTS
//------------------------------------------------------------------------------

// size of the room
const double roomLength = 6.0;	// x-axis
const double roomWidth = 3.0;	// y-axis
const double roomHeight = 1.5;	// z-axis

//------------------------------------------------------------------------------
// DECLARED MACROS
//------------------------------------------------------------------------------

// convert to resource path
#define RESOURCE_PATH(p)    (char*)((resourcesPath+string(p)).c_str())

// concatenates string q and p (e.g. q = "abc", p = "xyz" -> STR_ADD(q,p) = "abcxyz")
#define STR_ADD(q,p)		(char*)((string(q)+string(p)).c_str())

// initial position
#define INITIAL_POSITION	cVector3d(2.0, 0.0, 0.4)

// maximal number of objects
#define MAX_OBJECT_COUNT		20
#define MAX_AUDIOBUFFER_COUNT	MAX_OBJECT_COUNT
#define MAX_REGIONS_COUNT		MAX_OBJECT_COUNT

#endif