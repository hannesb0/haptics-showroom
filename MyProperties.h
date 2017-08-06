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

class MyProperties
{
public:

	//------------------------------------------------------------------------------
	// Public functions
	//------------------------------------------------------------------------------	

	// Constructor with parameters
	MyProperties(string setTexture, string setNormalMap, string setAudio, cVector3d setSize, MyOrientation setOrientation, enum MyShape setShape, int setTemperature,
		double setStiffness, double setStaticFriction, double setDynamicFriction, double setTextureLevel, double setAudioGain, double setAudioPitchGain, double setAudioPitchOffset);

	// Destructor
	~MyProperties();

	// show / get ID
	void showID();
	int getID();

	// show / get normal map
	void showNormal();
	string getNormal();

	// show / get texture
	void showTexture();
	string getTexture();

	//------------------------------------------------------------------------------
	// Public variables
	//------------------------------------------------------------------------------	

	// filename of image for graphical display 
	string textureImage;

	// filename of normal map for haptics display 
	string normalImage;

	// filename of audio file (for voice coil usage)
	string audio;

	// size
	cVector3d size;

	// orientation (see Global.h)
	MyOrientation orientation;

	// shape (see Global.h)
	enum MyShape shape;

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
	double audioPitchGain;

	// audio pitch offset
	double audioPitchOffset;

private:

	//------------------------------------------------------------------------------
	// Private variables
	//------------------------------------------------------------------------------	

	// property identifier
	static int id;
};

#endif // MYPROPERTIES_H_INCLUDED