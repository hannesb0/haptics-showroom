//==============================================================================
/*
Filename:	MyRegions.h
Project:	Haptics Showroom
Authors:	Naina Dhingra, Ke Xu, Hannes Bohnengel
Revision:	0.2
Remarks:	These files are tracked with git and are available on the github
repository: https://github.com/hannesb0/haptics-showroom
*/
//==============================================================================

#include "MyRegions.h"

// initialize object identifier
int MyRegions::id = 0;

/*
// Empty Constructor
MyRegions::MyRegions()
{
	id++;
}
*/

// Constructor with parameters
MyRegions::MyRegions(cVector3d setPosition, cVector3d setSize, int setTemperature)
{
	// setting the position
	position = setPosition;

	// setting the size
	size = setSize;

	// asigning a temperature
	temperature = setTemperature;

	// set active to false
	//active = false;

	// increase counter
	id++;
}

// Destructor
MyRegions::~MyRegions()
{
	// decrease identifier
	if (id > 0)
		id--;
}

void MyRegions::showID()
{
	cout << "Current property ID: " << id << endl;
}

int MyRegions::getID()
{
	return id;
}