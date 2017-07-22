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
int MyProperties::id = 0;

// Empty Constructor
MyProperties::MyProperties()
{
	// set the resources path
	//resourcesPath = path;

	// set default filename of image for graphical display 
	textureImage = defaultTextureImage;

	// set default filename of normal map for haptics display 
	normalImage = defaultNormalImage;

	// set default temperature of the object
	temperature = 3;

	// default stiffness of the object
	stiffness = defaultStiffness;

	// default staticFriction of the object
	staticFriction = 0.2;

	// default dynamicFriction of the object
	dynamicFriction = 0.2;

	// default texture level
	textureLevel = 0.1;

	// default audio friction gain
	audioGain = 0;

	// default audio pitch
	audioPitch = 0.2;

	// increase identifier
	id++;
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
	id++;
}

// Destructor
MyProperties::~MyProperties()
{
	// decrease identifier
	if (id > 0)
		id--;
}

void MyProperties::showID()
{
	cout << "Current object ID: " << id << endl;
}

int MyProperties::getID()
{
	return id;
}

void MyProperties::showNormal()
{
	cout << "Current object normal map: " <<  normalImage << endl;
}

string MyProperties::getNormal()
{
	return normalImage;
}

void MyProperties::showTexture()
{
	cout << "Current object Texture: " << textureImage << endl;
}

string MyProperties::getTexture()
{
	return textureImage;
}