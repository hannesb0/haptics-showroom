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
#include "Global.h"

using namespace std;

// mStickSlipForceMax, mStickSlipStiffness,

class MyProperties
{
public:

	//------------------------------------------------------------------------------
	// Public functions
	//------------------------------------------------------------------------------	

	// Empty Constructor
	MyProperties();

	// Constructor with parameters
	MyProperties(string setTexture, string setNormalMap, int setTemperature, double setStiffness, double setStaticFriction, double setDynamicFriction, double setTextureLevel, double setAudioGain, double setAudioPitch);

	// Destructor
	~MyProperties();

	void showID();
	int getID();

	void showNormal();
	string getNormal();

	void showTexture();
	string getTexture();


	//bool setNormalMap();

	//bool setTexture();

	//------------------------------------------------------------------------------
	// Public variables
	//------------------------------------------------------------------------------	

	// filename of image for graphical display 
	string textureImage;

	// filename of normal map for haptics display 
	string normalImage;

	// temperature of the object (the temperature is divided into 5 areas: 1 = very cold, 2 = cold, 3 = normal, 4 = hot, 5 = very hot)
	int temperature;

	// stiffness of the object (this is the percentage of the maximal stiffness (0 < stiffness < 1.0)
	double stiffness;

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

private:

	//------------------------------------------------------------------------------
	// Private variables
	//------------------------------------------------------------------------------	

	// property identifier
	static int id;
};

#endif // MYPROPERTIES_H_INCLUDED