//==============================================================================
/*
Filename:	MyObject.cpp
Project:	Haptics Showroom
Authors:	Naina Dhingra, Ke Xu, Hannes Bohnengel
Revision:	0.2
Remarks:	These files are tracked with git and are available on the github
			repository: https://github.com/hannesb0/haptics-showroom
*/
//==============================================================================

#include "MyObject.h"

// initialize object identifier
//int MyObject::id = 0;

/*
// Empty Constructor
MyObject::MyObject()
{

}
*/

// Constructor with parameters
MyObject::MyObject(cVector3d setSize, enum MyShape setShape, MyProperties setProperties)
{
	// set size
	size = setSize;

	// set shape
	shape = setShape;

	// set properties
	properties = setProperties;

	// increase identifier
	//id++;
}

// Destructor
MyObject::~MyObject()
{
	// decrease identifier
	//if (id > 0)
	//	id--;
}

cVector3d MyObject::getPosition()
{
	return position;
}
void MyObject::setPosition(cVector3d setPosition)
{
	position = setPosition;
}

void MyObject::getOrientation(cVector3d *getAxis, double *getRotation)
{
	*getAxis = axis;
	*getRotation = rotation;
}
void MyObject::setOrientation(cVector3d setAxis, double setRotation)
{
	axis = setAxis;
	rotation = setRotation;
}

/*
void MyObject::showID()
{
	cout << "Current object ID: " << id << endl;
}

int MyObject::getID()
{
	return id;
}
*/