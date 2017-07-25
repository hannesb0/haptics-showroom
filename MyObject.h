//==============================================================================
/*
Filename:	MyObject.h
Project:	Haptics Showroom
Authors:	Naina Dhingra, Ke Xu, Hannes Bohnengel
Revision:	0.2
Remarks:	These files are tracked with git and are available on the github
repository: https://github.com/hannesb0/haptics-showroom
*/
//==============================================================================

#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "Global.h"
#include "MyProperties.h"

using namespace std;

class MyObject
{
public:

	//------------------------------------------------------------------------------
	// Public functions
	//------------------------------------------------------------------------------

	// Empty Constructor
	//MyObject();

	// Constructor with parameters
	MyObject(cVector3d setSize, enum MyShape setShape, MyProperties setProperties);

	// Destructor
	~MyObject();

	// get / show ID of object
	//void showID();
	//int getID();

	// get / set position of object
	cVector3d getPosition();
	void setPosition(cVector3d setPosition);

	// get / set orientation of object
	void getOrientation(cVector3d *getAxis, double *getRotation);
	void setOrientation(cVector3d setAxis, double setRotation);

	//------------------------------------------------------------------------------
	// Public variables
	//------------------------------------------------------------------------------

	// object describers
	cVector3d		position;
	cVector3d		size;
	cVector3d		axis;
	double			rotation;
	enum MyShape	shape;
	MyProperties	properties;

private:

	//------------------------------------------------------------------------------
	// Private variables
	//------------------------------------------------------------------------------

	//static int id;
};

#endif // MYOBJECT_H_INCLUDED