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
MyProperties::MyProperties(string path)
{
	// set the resources path
	resourcesPath = path;

	// set default filename of image for graphical display 
	//textureImage = "brick-color.png";

	// set default filename of normal map for haptics display 
	//normalImage = "brick-normal.png";

	// set default normal map (chai3d specific object)
	(*normalMap)->loadFromFile(RESOURCE_PATH("images/brick-normal.png"));;

	// set default texture (chai3d specific object)
	(*texture)->loadFromFile(RESOURCE_PATH("images/brick-color.png"));

	// set default size of the object
	//size = cVector3d(0.2, 0.2, 0.2);

	// set default temperature of the object
	temperature = 3;

	// default stiffness of the object
	stiffness = 0.3;

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
MyProperties::MyProperties(cTexture2dPtr *setTexture, cNormalMapPtr *setNormalMap, int setTemperature, int setStiffness, double setStaticFriction, double setDynamicFriction, double setTextureLevel, double setAudioGain, double setAudioPitch)
{
	// set filename of image for graphical display 
	//textureImage = setTextureImage;

	// set filename of normal map for haptics display 
	//normalImage = setNormalImage;

	// set normal map (chai3d specific object)
	normalMap = setNormalMap;

	// set texture (chai3d specific object)
	texture = setTexture;

	// set size of the object
	//size = cVector3d(0.2, 0.2, 0.2);

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