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

#ifndef MYREGIONS_H
#define MYREGIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "Global.h"

//using namespace std;

class MyRegions
{
public:

	//------------------------------------------------------------------------------
	// Public functions
	//------------------------------------------------------------------------------	

	// Empty Constructor
	//MyRegions();

	// Constructor with parameters
	MyRegions(cVector3d setPosition, cVector3d setSize, int setTemperature);

	// Destructor
	~MyRegions();

	// show / get ID
	void showID();
	int getID();

	//------------------------------------------------------------------------------
	// Public variables
	//------------------------------------------------------------------------------	

	// position of the region
	cVector3d position;

	// size of the region
	cVector3d size;

	// assigned temperature of the region
	int temperature;

	// active flag
	//bool active;

private:

	//------------------------------------------------------------------------------
	// Private variables
	//------------------------------------------------------------------------------	

	// property identifier
	static int id;
};

#endif // MYREGIONS_H INCLUDED