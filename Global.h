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
string defaultTextureImage = "brick-color.png";

// default filename of normal map for haptics display 
string defaultNormalImage = "brick-normal.png";


//------------------------------------------------------------------------------
// DECLARED MACROS
//------------------------------------------------------------------------------

// convert to resource path
#define RESOURCE_PATH(p)    (char*)((resourcesPath+string(p)).c_str())

#endif