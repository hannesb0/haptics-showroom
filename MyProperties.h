//==============================================================================
/*
Filename:	MyProperties.h
Project:	Haptics Showroom
Authors:	Naina Dhingra, Ke Xu, Hannes Bohnengel
Revision:	0.2
Remarks:	These files are tracked with git and are available on the github
repository: https://github.com/hannesb0/haptics-showroom
*/
//==============================================================================

#ifndef MYPROPERTIES_H
#define MYPROPERTIES_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "chai3d.h"			// is size a member of this class ???????????????

using namespace chai3d;		// is size a member of this class ???????????????
using namespace std;

// convert to resource path
#define RESOURCE_PATH(p)    (char*)((resourcesPath+string(p)).c_str())





// mTextureLevel, mStickSlipForceMax, mStickSlipStiffness, mAudioFrictionGain - 





class MyProperties
{
public:

	// Empty Constructor
	MyProperties(string path);

	// Constructor with parameters
	MyProperties(cTexture2dPtr *setTexture, cNormalMapPtr *setNormalMap, int setTemperature, int setStiffness, double setStaticFriction, double setDynamicFriction, double setTextureLevel, double setAudioGain, double setAudioPitch);

	// Destructor
	~MyProperties();

	//------------------------------------------------------------------------------
	// Variables
	//------------------------------------------------------------------------------	

	// property identifier
	static int id;

	// filename of image for graphical display 
	//string textureImage;

	// filename of normal map for haptics display 
	//string normalImage;
	
	// normal map (chai3d specific object)
	cNormalMapPtr *normalMap;

	// texture (chai3d specific object)
	cTexture2dPtr *texture;

	// size of the object
	//cVector3d size;

	// temperature of the object (the temperature is divided into 5 areas: 1 = very cold, 2 = cold, 3 = normal, 4 = hot, 5 = very hot)
	int temperature;

	// stiffness of the object (this is the percentage of the maximal stiffness (0 < stiffness < 1.0)
	int stiffness;

	// staticFriction of the object
	double staticFriction;

	// dynamicFriction of the object
	double dynamicFriction;

	// texture level
	double textureLevel;

	// audio gain
	double audioGain;

	// audio pitch
	double audioPitch;

	//------------------------------------------------------------------------------
	// Functions
	//------------------------------------------------------------------------------	

	//bool setNormalMap();

	//bool setTexture();

private:

	string resourcesPath;
	
};

#endif // MYPROPERTIES_H_INCLUDED