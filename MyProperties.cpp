//==============================================================================
/*
Filename:	MyProperties.cpp
Project:	Haptics Showroom
Authors:	Naina Dhingra, Ke Xu, Hannes Bohnengel
Revision:	0.2
Remarks:	These files are tracked with git and are available on the github
			repository: https://github.com/hannesb0/haptics-showroom
*/
//==============================================================================

#include "MyProperties.h"

// initialize object identifier
//int MyProperties::id = 0;

// Empty Constructor
MyProperties::MyProperties()
{
	// set the resources path
	//resourcesPath = path;

	// set default filename of image for graphical display 
	textureImage = defaultTextureImage;

	// set default filename of normal map for haptics display 
	normalImage = defaultNormalImage;

	// set default audio file (for voice coil)
	audio = defaultAudio;

	// set default temperature of the object
	temperature = defaultTemperature;

	// set default stiffness of the object
	stiffness = defaultStiffness;

	// set default staticFriction of the object
	staticFriction = defaultStaticFriction;

	// set default dynamicFriction of the object
	dynamicFriction = defaultDynamicFriction;

	// set default texture level
	textureLevel = defaultTextureLevel;

	// set default audio friction gain
	audioGain = defaultAudioGain;

	// set default audio pitch
	audioPitch = defaultAudioPitch;

	// increase identifier
	//id++;
}

// Constructor with parameters
MyProperties::MyProperties(string setTexture, string setNormalMap, int setTemperature, double setStiffness, double setStaticFriction, double setDynamicFriction, double setTextureLevel, double setAudioGain, double setAudioPitch)
{
	// set filename of image for graphical display 
	textureImage = setTexture;

	// set filename of normal map for haptics display 
	normalImage = setNormalMap;

	// set temperature of the object
	temperature = setTemperature;

	// set stiffness of the object
	stiffness = setStiffness;

	// set staticFriction of the object
	staticFriction = setStaticFriction;

	// set dynamicFriction of the object
	dynamicFriction = setDynamicFriction;

	// set texture level
	textureLevel = setTextureLevel;

	// set audio friction gain
	audioGain = setAudioGain;

	// set audio pitch
	audioPitch = setAudioPitch;

	// increase identifier
	//id++;
}

// Destructor
MyProperties::~MyProperties()
{
	// decrease identifier
	//if (id > 0)
	//	id--;
}
/*
void MyProperties::showID()
{
	cout << "Current property ID: " << id << endl;
}

int MyProperties::getID()
{
	return id;
}
*/
void MyProperties::showNormal()
{
	cout << "Current normal map: " <<  normalImage << endl;
}

string MyProperties::getNormal()
{
	return normalImage;
}

void MyProperties::showTexture()
{
	cout << "Current texture: " << textureImage << endl;
}

string MyProperties::getTexture()
{
	return textureImage;
}