//==============================================================================
/*
	Filename:	haptics_showroom-V02.cpp
	Project:	Haptics Showroom
    Authors:	Naina Dhingra, Ke Xu, Hannes Bohnengel 
    Revision:	0.2
	Remarks:	These files are tracked with git and are available on the github
				repository: https://github.com/hannesb0/haptics-showroom
*/
//==============================================================================

//------------------------------------------------------------------------------
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//------------------------------------------------------------------------------
#include "chai3d.h"
//------------------------------------------------------------------------------
#include "SDL.h"
#include "OVRRenderContext.h"
#include "OVRDevice.h"
//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;
//------------------------------------------------------------------------------


////////////////////////////////////////
// define if Oculus Rift is used or not
bool useOculus = false;
////////////////////////////////////////


//------------------------------------------------------------------------------
// DECLARED MACROS
//------------------------------------------------------------------------------

// convert to resource path
#define RESOURCE_PATH(p)    (char*)((resourceRoot+string(p)).c_str())
#define WINDOW_SIZE_W		1000
#define WINDOW_SIZE_H		1000
#define TOOL_RADIUS			0.02
#define TOOL_WORKSPACE		0.3
#define INITIAL_POSITION	cVector3d(2.0, 0.0, 0.2)

//------------------------------------------------------------------------------
// CHAI3D
//------------------------------------------------------------------------------

// a world that contains all objects of the virtual environment
cWorld* world;

// a camera to render the world in the window display
cCamera* camera;

// a light source
//cSpotLight *light;
cDirectionalLight *light;

// a haptic device handler
cHapticDeviceHandler* handler;

// a pointer to the current haptic device
cGenericHapticDevicePtr hapticDevice;
cGenericHapticDevicePtr hapticDevice2;

// a virtual tool representing the haptic device in the scene
cToolCursor* tool;
cToolCursor* tool2;

// a virtual mesh like object
cMesh* object;
cMesh* objectX;
// a mesh model
cMultiMesh* object1;
cMultiMesh* object2;
cMultiMesh* object3;
cMultiMesh* object4;
cMultiMesh* object5;

// model of earth
cMesh* earth;

// model of starts
cMesh* globe;


// indicates if the haptic simulation currently running
bool simulationRunning = false;

// indicates if the haptic simulation has terminated
bool simulationFinished = false;

// frequency counter to measure the simulation haptic rate
cFrequencyCounter frequencyCounter;

// new path to the resources (included in the repository)
string resourcesPath, resourceRoot;

// initial position : on +Z //change for testing to: cVector3d(1.0, 0.0, 0.2) For Presentation:cVector3d(2.0, 0.0, 0.2)
cVector3d currentPosition = INITIAL_POSITION;
cVector3d currentDirection = cVector3d(1.0, 0.0, 0.0);
cVector3d deviceOffset = cVector3d(0.2, 0.0, 0.0);

// if two haptic devices are used they need to be separated in space
cVector3d deviceOffset1 = cVector3d(0.2, -0.1, 0.0);
cVector3d deviceOffset2 = cVector3d(0.2, 0.1, 0.0);

// variable for changing the perspective and for walking
double currentAngle = 0;
double speed = 0.004;
double rotationalSpeed = 0.006;

// variable to store state of keys
unsigned int keyState[255];

//------------------------------------------------------------------------------
// OCULUS RIFT
//------------------------------------------------------------------------------

// display context
cOVRRenderContext renderContext;

// oculus device
cOVRDevice oculusVR;


// ------------------------------------------------------------------------------
// DECLARED MACROS
//------------------------------------------------------------------------------

// convert to resource path
#define RESOURCE_PATH(p)    (char*)((resourceRoot+string(p)).c_str())


//------------------------------------------------------------------------------
// DECLARED FUNCTIONS
//------------------------------------------------------------------------------

// function that closes the application
void close(void);

// main haptics simulation loop
void updateHaptics(void);

// process keyboard events
void processEvents();

// function that computes movements through key input
void computeMatricesFromInput();

// function to create a new object at runtime
int new_object(int argc, char **argv, cVector3d position, int property);

// for testing purposes !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
int new_wall(int argc, char **argv);
void delete_object();


//==============================================================================
// MAIN FUNCTION
//==============================================================================

int main(int argc, char **argv)
{
	//--------------------------------------------------------------------------
	// INITIALIZATION
	//--------------------------------------------------------------------------

	cout << endl;
	cout << "-----------------------------------" << endl;
	cout << "CHAI3D" << endl;
	cout << "Project: Haptics Showroom" << endl;
	cout << "Team:    Naina Dhingra, Ke Xu, Hannes Bohnengel" << endl;
	cout << "Rev.:    0.2" << endl;
	cout << "-----------------------------------" << endl << endl << endl;
	cout << "Keyboard Options:" << endl << endl;
	cout << "Space  - Recenter view point" << endl;
	cout << "Escape - Exit application" << endl;
	cout << "OTHERS ARE FOLLOWING AND NEED TO BE DEFINED" << endl;
	cout << endl << endl;

	// parse first arg to try and locate resources
	resourceRoot = string(argv[0]).substr(0, string(argv[0]).find_last_of("/\\") + 1);
	resourcesPath = resourceRoot + string("../../examples/haptics-showroom-V02/resources/");

	//--------------------------------------------------------------------------
	// SETUP DISPLAY CONTEXT
	//--------------------------------------------------------------------------

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
	{
		cout << "failed initialization" << endl;
		cSleepMs(1000);
		return 1;
	}

	// check if Oculus should be used
	if (!oculusVR.initVR() && useOculus)
	{
		cout << "failed to initialize Oculus" << endl;
		cSleepMs(1000);
		SDL_Quit();
		return 1;
	}

	// check if Oculus should be used
	if (useOculus)
	{
		ovrSizei hmdResolution = oculusVR.getResolution();
		ovrSizei windowSize = { hmdResolution.w / 2, hmdResolution.h / 2 };
	}
	else
	{
		renderContext.init("CHAI3D", 100, 100, WINDOW_SIZE_W, WINDOW_SIZE_H);
	}

	SDL_ShowCursor(SDL_DISABLE);

	if (glewInit() != GLEW_OK)
	{
		// check if Oculus should be used
		if (useOculus)
		{
			oculusVR.destroyVR();
			renderContext.destroy();
		}
		SDL_Quit();
		return 1;
	}

	// check if Oculus should be used
	if (useOculus)
	{
		ovrSizei hmdResolution = oculusVR.getResolution();
		ovrSizei windowSize = { hmdResolution.w / 2, hmdResolution.h / 2 };
		if (!oculusVR.initVRBuffers(windowSize.w, windowSize.h))
		{
			oculusVR.destroyVR();
			renderContext.destroy();
			SDL_Quit();
			return 1;
		}
	}


	//--------------------------------------------------------------------------
	// WORLD - CAMERA - LIGHTING
	//--------------------------------------------------------------------------

	// create a new world.
	world = new cWorld();

	// set the background color of the environment
	// the color is defined by its (R,G,B) components.
	world->m_backgroundColor.setWhite();

	// create a camera and insert it into the virtual world
	camera = new cCamera(world);
	world->addChild(camera);

	// position and orient the camera
	camera->set(currentPosition,    // camera position (eye)
		cVector3d(0.0, 0.0, 0.0),    // lookat position (target)
		cVector3d(0.0, 0.0, 1.0));   // direction of the "up" vector

	// set the near and far clipping planes of the camera
	// anything in front/behind these clipping planes will not be rendered
	camera->setClippingPlanes(0.01, 20.0);

	// create a light source
	//light = new cSpotLight(world);
	light = new cDirectionalLight(world);

	// add light to world
	world->addChild(light);

	// enable light source
	light->setEnabled(true);

	// position the light source
	light->setLocalPos(3.5, 2.0, 0.0);

	// define the direction of the light beam
	//light->setDir(-3.5, -2.0, 0.0);
	light->setDir(0.0, 0.0, -30.0);

	// set light cone half angle
	//light->setCutOffAngleDeg(50);
	light->m_ambient.set(0.4f, 0.4f, 0.4f);
	light->m_diffuse.set(0.8f, 0.8f, 0.8f);
	light->m_specular.set(1.0f, 1.0f, 1.0f);


	//--------------------------------------------------------------------------
	// HAPTIC DEVICES / TOOLS
	//--------------------------------------------------------------------------

	// create a haptic device handler
	handler = new cHapticDeviceHandler();

	// get access to the first available haptic device
	handler->getDevice(hapticDevice, 0);
	handler->getDevice(hapticDevice2, 1);

	// retrieve information about the current haptic device
	cHapticDeviceInfo hapticDeviceInfo = hapticDevice->getSpecifications();
	cHapticDeviceInfo hapticDeviceInfo2 = hapticDevice2->getSpecifications();

	// create a 3D tool and add it to the world
	tool = new cToolCursor(world);
	world->addChild(tool);
	tool2 = new cToolCursor(world);
	world->addChild(tool2);

	// connect the haptic device to the tool
	tool->setHapticDevice(hapticDevice);
	tool2->setHapticDevice(hapticDevice2);

	// define the radius of the tool (sphere)
	double toolRadius = TOOL_RADIUS;

	// define a radius for the tool
	tool->setRadius(TOOL_RADIUS);
	tool2->setRadius(TOOL_RADIUS);

	// enable if objects in the scene are going to rotate of translate
	// or possibly collide against the tool. If the environment
	// is entirely static, you can set this parameter to "false"
	tool->enableDynamicObjects(true);
	tool2->enableDynamicObjects(true);

	tool->enableDynamicObjects(true);
	// map the physical workspace of the haptic device to a larger virtual workspace.
	tool->setWorkspaceRadius(TOOL_WORKSPACE);
	tool2->setWorkspaceRadius(TOOL_WORKSPACE);

	// haptic forces are enabled only if small forces are first sent to the device;
	// this mode avoids the force spike that occurs when the application starts when 
	// the tool is located inside an object for instance. 
	tool->setWaitForSmallForce(true);
	tool2->setWaitForSmallForce(true);

	// start the haptic tool
	tool->start();
	tool2->start();


	//--------------------------------------------------------------------------
	// CREATE OBJECT
	//--------------------------------------------------------------------------

	// read the scale factor between the physical workspace of the haptic
	// device and the virtual workspace defined for the tool
	double workspaceScaleFactor = tool->getWorkspaceScaleFactor();

	// stiffness properties
	double maxStiffness = hapticDeviceInfo.m_maxLinearStiffness / workspaceScaleFactor;

	// create a virtual mesh
	object = new cMesh();

	// add object to world
	world->addChild(object);

	// set the position of the object at the center of the world
	object->setLocalPos(-10.0, 20.0, -20.0);

	// create cube
	cCreateBox(object, 0.2, 0.2, 0.2);

	// create a texture
	cTexture2dPtr texture = cTexture2d::create();

	bool fileload = texture->loadFromFile(RESOURCE_PATH("../resources/images/brick-color.png"));
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = texture->loadFromFile("../../../bin/resources/images/brick-color.png");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
		close();
		return (-1);
	}

	// apply texture to object
	object->setTexture(texture);

	// enable texture rendering 
	object->setUseTexture(true);

	// Since we don't need to see our polygons from both sides, we enable culling.
	object->setUseCulling(true);

	// set material properties to light gray
	object->m_material->setWhite();

	// compute collision detection algorithm
	object->createAABBCollisionDetector(TOOL_RADIUS);

	// define a default stiffness for the object
	object->m_material->setStiffness(0.3 * maxStiffness);

	// define some static friction
	object->m_material->setStaticFriction(0.2);

	// define some dynamic friction
	object->m_material->setDynamicFriction(0.2);

	// define some texture rendering
	object->m_material->setTextureLevel(0.1);

	// render triangles haptically on front side only
	object->m_material->setHapticTriangleSides(true, false);

	// create a normal texture
	cNormalMapPtr normalMap = cNormalMap::create();

	// load normal map from file
	fileload = normalMap->loadFromFile(RESOURCE_PATH("../resources/images/brick-normal.png"));
	if (!fileload)
	{
	#if defined(_MSVC)
		fileload = normalMap->loadFromFile("../../../bin/resources/images/brick-normal.png");
	#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
		close();
		return (-1);
	}

	// assign normal map to object
	object->m_normalMap = normalMap;

	// compute surface normals
	object->computeAllNormals();

	// compute tangent vectors
	object->m_triangles->computeBTN();

	//--------------------------------------------------------------------------
	// CREATE OBJECT table
	//--------------------------------------------------------------------------

	// read the scale factor between the physical workspace of the haptic
	// device and the virtual workspace defined for the tool
	double workspaceScaleFactor1 = tool->getWorkspaceScaleFactor();

	// stiffness properties
	double maxStiffness1 = hapticDeviceInfo.m_maxLinearStiffness / workspaceScaleFactor1;

	// create a virtual mesh
	object1 = new cMultiMesh();

	// add object to world
	world->addChild(object1);
	// set the position of the object at the center of the world
	object1->setLocalPos(-100.0, -200.0, -100.0);

	// rotate the object 90 degrees
	object1->rotateAboutGlobalAxisDeg(cVector3d(0.5, 0.5, 1), 100);

	// load an object file
	bool fileload1;
	fileload1 = object1->loadFromFile(RESOURCE_PATH("../resources/models/table/simpleTable.3ds"));
	if (!fileload)
	{
#if defined(_MSVC)
		fileload1 = object1->loadFromFile("../../../bin/resources/models/table/simpleTable.3ds");
#endif
	}
	if (!fileload1)
	{
		cout << "Error - 3D Model failed to load correctly." << endl;
		close();
		return (-1);
	}

	// get dimensions of object
	object1->computeBoundaryBox(true);
	double size = cSub(object1->getBoundaryMax(), object1->getBoundaryMin()).length();

	// resize object to screen
	if (size > 0.001)
	{
		object1->scale(1/size);
	}

	// compute collision detection algorithm
	object1->createAABBCollisionDetector(.02);

	// disable culling so that faces are rendered on both sides
	object1->setUseCulling(false);

	// enable display list for faster graphic rendering
	object1->setUseDisplayList(true);

	// center object in scene
	object1->setLocalPos(-1.0 * object->getBoundaryCenter());

	// rotate object in scene
	object1->rotateExtrinsicEulerAnglesDeg(0, 0, 90, C_EULER_ORDER_XYZ);

	// set haptic properties
	cMaterial mat;
	mat.setHapticTriangleSides(true, true);
	mat.setStiffness(0.2 * maxStiffness);
	mat.setStaticFriction(0.2);
	mat.setDynamicFriction(0.1);
	object1->setMaterial(mat);

	//--------------------------------------------------------------------------
	// CREATE OBJECT
	//--------------------------------------------------------------------------
//
//	// read the scale factor between the physical workspace of the haptic
//	// device and the virtual workspace defined for the tool
//	double workspaceScaleFactor2 = tool->getWorkspaceScaleFactor();
//
//	// stiffness properties
//	double maxStiffness2 = hapticDeviceInfo.m_maxLinearStiffness / workspaceScaleFactor2;
//
//	// create a virtual mesh
//	object2 = new cMultiMesh();
//
//	// add object to world
//	world->addChild(object2);
//
//	// set the position of the object at the center of the world
//	object2->setLocalPos(5.0, 5.0, 0.0);
//
//	//// rotate the object 90 degrees
//	//object->rotateAboutGlobalAxisDeg(cVector3d(0, 0, 1), 90);
//	// rotate the object 90 degrees
//	object2->rotateAboutGlobalAxisDeg(cVector3d(0.5, 0.5, 1), 100);
//
//
//	// load an object file
//	bool fileload3;
//	fileload3 = object2->loadFromFile(RESOURCE_PATH("../resources/models/eagle 2/EAGLE_2.3DS"));
//	if (!fileload)
//	{
//#if defined(_MSVC)
//		fileload3 = object2->loadFromFile("../../../bin/resources/models/eagle 2/EAGLE_2.3DS");
//#endif
//	}
//	if (!fileload3)
//	{
//		cout << "Error - 3D Model failed to load correctly." << endl;
//		close();
//		return (-1);
//	}
//
//	// compute a boundary box
//	object2->computeBoundaryBox(true);
//
//	// get dimensions of object
//	double size2 = cSub(object2->getBoundaryMax(), object2->getBoundaryMin()).length();
//
//	// resize object to screen
//	if (size2 > 0)
//	{
//		object2->scale(10.0 * tool->getWorkspaceRadius() / size2);
//	}
//
//	// compute collision detection algorithm
//	object2->createAABBCollisionDetector(toolRadius);
//
//	// enable haptic rendering on both sides of triangles
//	cMaterial mat2;
//	mat2.setHapticTriangleSides(true, true);
//	object2->setMaterial(mat2);
//
//	// define some environmental texture mapping
//	cTexture2dPtr texture2 = cTexture2d::create();
//
//	// load texture file
//	fileload3 = texture2->loadFromFile(RESOURCE_PATH("../resources/images/chrome.jpg"));
//	if (!fileload3)
//	{
//#if defined(_MSVC)
//		fileload3 = texture2->loadFromFile("../../../bin/resources/images/chrome.jpg");
//#endif
//	}
//	if (!fileload3)
//	{
//		cout << "Error - Texture2 image failed to load correctly." << endl;
//		close();
//		return (-1);
//	}
//
//	// enable spherical mapping
//	texture2->setSphericalMappingEnabled(true);
//
//	// assign texture to object
//	object2->setTexture(texture, true);
//
//	// enable texture mapping
//	object2->setUseTexture(true, true);
//
//	// disable culling
//	object2->setUseCulling(false, true);
//
//	// enable display list for faster graphic rendering
//	object2->setUseDisplayList(true);
//
//	// define a default stiffness for the object
//	object2->setStiffness(maxStiffness, true);
//
//	// define some haptic friction properties
//	object2->setFriction(0.1, 0.2, true);
//
//--------------------------------------------------------------------------
// CREATE OBJECT bird
//--------------------------------------------------------------------------

	// read the scale factor between the physical workspace of the haptic
	// device and the virtual workspace defined for the tool
	double workspaceScaleFactor2 = tool->getWorkspaceScaleFactor();

	// stiffness properties
	double maxStiffness2 = hapticDeviceInfo.m_maxLinearStiffness / workspaceScaleFactor2;

	// create a virtual mesh
	object2 = new cMultiMesh();

	// add object to world
	world->addChild(object2);
	// set the position of the object at the center of the world
	object2->setLocalPos(1000.0, 3000.0, 3000.0);


	// rotate the object 90 degrees
	object2->rotateAboutGlobalAxisDeg(cVector3d(-0.5, -0.5, -1), 100);

	// load an object file
	bool fileload2;
	fileload2 = object2->loadFromFile(RESOURCE_PATH("../resources/models/eagle 2/EAGLE_2.3DS"));
	if (!fileload)
	{
#if defined(_MSVC)
		fileload2 = object2->loadFromFile("../../../bin/resources/models/eagle 2/EAGLE_2.3DS");
#endif
	}
	if (!fileload2)
	{
		cout << "Error - 3D Model failed to load correctly." << endl;
		close();
		return (-1);
	}

	// get dimensions of object
	object2->computeBoundaryBox(true);
	double size2 = cSub(object2->getBoundaryMax(), object2->getBoundaryMin()).length();

	// resize object to screen
	if (size2 > 0.001)
	{
		object2->scale(1/size2);
	}

	// compute collision detection algorithm
	object2->createAABBCollisionDetector(.02);

	// disable culling so that faces are rendered on both sides
	object2->setUseCulling(false);

	// enable display list for faster graphic rendering
	object2->setUseDisplayList(true);

	// center object in scene
	object2->setLocalPos(-1.0 * object2->getBoundaryCenter());

	// rotate object in scene
	object2->rotateExtrinsicEulerAnglesDeg(0, 0, 90, C_EULER_ORDER_XYZ);

	// set haptic properties
	cMaterial mat2;
	mat2.setHapticTriangleSides(true, true);
	mat2.setStiffness(0.2 * maxStiffness2);
	mat2.setStaticFriction(0.2);
	mat2.setDynamicFriction(0.1);
	object2->setMaterial(mat2);

	//--------------------------------------------------------------------------
	// CREATE OBJECT boy
	//--------------------------------------------------------------------------

	// read the scale factor between the physical workspace of the haptic
	// device and the virtual workspace defined for the tool
	double workspaceScaleFactor3 = tool->getWorkspaceScaleFactor();

	// stiffness properties
	double maxStiffness3 = hapticDeviceInfo.m_maxLinearStiffness / workspaceScaleFactor3;

	// create a virtual mesh
	object3 = new cMultiMesh();

	// add object to world
	world->addChild(object3);
	// set the position of the object at the center of the world
	object3->setLocalPos(-1000.0, -3000.0, -3000.0);


	// rotate the object 90 degrees
	object3->rotateAboutGlobalAxisDeg(cVector3d(-0.5, -0.5, -1), 100);

	// load an object file
	bool fileload0;
	fileload0 = object3->loadFromFile(RESOURCE_PATH("../resources/models/Boy/boy.3ds"));
	if (!fileload)
	{
#if defined(_MSVC)
		fileload0 = object3->loadFromFile("../../../bin/resources/models/Boy/boy.3ds");
#endif
	}
	if (!fileload0)
	{
		cout << "Error - 3D Model failed to load correctly." << endl;
		close();
		return (-1);
	}

	// get dimensions of object
	object3->computeBoundaryBox(true);
	double size3 = cSub(object3->getBoundaryMax(), object3->getBoundaryMin()).length();

	// resize object to screen
	if (size3 > 0.001)
	{
		object3->scale(1 / size3);
	}

	// compute collision detection algorithm
	object3->createAABBCollisionDetector(.02);

	// disable culling so that faces are rendered on both sides
	object3->setUseCulling(false);

	// enable display list for faster graphic rendering
	object3->setUseDisplayList(true);

	// center object in scene
	object3->setLocalPos(-1.0 * object3->getBoundaryCenter());

	// rotate object in scene
	object3->rotateExtrinsicEulerAnglesDeg(0, 0, 90, C_EULER_ORDER_XYZ);

	// set haptic properties
	cMaterial mat3;
	mat3.setHapticTriangleSides(true, true);
	mat3.setStiffness(0.2 * maxStiffness2);
	mat3.setStaticFriction(0.2);
	mat3.setDynamicFriction(0.1);
	object2->setMaterial(mat3);

	//--------------------------------------------------------------------------
	// CREATE OBJECT
	//--------------------------------------------------------------------------

	// read the scale factor between the physical workspace of the haptic
	// device and the virtual workspace defined for the tool
	double workspaceScaleFactor4 = tool->getWorkspaceScaleFactor();

	// stiffness properties
	double maxStiffness4 = hapticDeviceInfo.m_maxLinearStiffness / workspaceScaleFactor4;

	// create a virtual mesh
	object4 = new cMultiMesh();

	// add object to world
	world->addChild(object4);
	// set the position of the object at the center of the world
	object4->setLocalPos(0.5,0.7,0.9);


	// rotate the object 90 degrees
	object4->rotateAboutGlobalAxisDeg(cVector3d(-0.5, -0.5, -1), 100);

	// load an object file
	bool fileload4;
	fileload4 = object4->loadFromFile(RESOURCE_PATH("../resources/models/Old Lantern Iridesum/Old Lantern Model.3ds"));
	if (!fileload)
	{
#if defined(_MSVC)
		fileload4 = object4->loadFromFile("../../../bin/resources/models/Old Lantern Iridesum/Old Lantern Model.3ds");
#endif
	}
	if (!fileload4)
	{
		cout << "Error - 3D Model failed to load correctly." << endl;
		close();
		return (-1);
	}

	// get dimensions of object
	object4->computeBoundaryBox(true);
	double size4 = cSub(object4->getBoundaryMax(), object4->getBoundaryMin()).length();

	// resize object to screen
	if (size4 > 0.001)
	{
		object4->scale(1 / size3);
	}

	// compute collision detection algorithm
	object4->createAABBCollisionDetector(.02);

	// disable culling so that faces are rendered on both sides
	object4->setUseCulling(false);

	// enable display list for faster graphic rendering
	object4->setUseDisplayList(true);

	// center object in scene
	object4->setLocalPos(-1.0 * object4->getBoundaryCenter());

	// rotate object in scene
	object4->rotateExtrinsicEulerAnglesDeg(0, 0, 90, C_EULER_ORDER_XYZ);

	// set haptic properties
	cMaterial mat4;
	mat4.setHapticTriangleSides(true, true);
	mat4.setStiffness(0.2 * maxStiffness2);
	mat4.setStaticFriction(0.2);
	mat4.setDynamicFriction(0.1);
	object4->setMaterial(mat4);
	//--------------------------------------------------------------------------
	// CREATE OBJECT car
	//--------------------------------------------------------------------------

	// read the scale factor between the physical workspace of the haptic
	// device and the virtual workspace defined for the tool
	double workspaceScaleFactor5 = tool->getWorkspaceScaleFactor();

	// stiffness properties
	double maxStiffness5 = hapticDeviceInfo.m_maxLinearStiffness / workspaceScaleFactor5;

	// create a virtual mesh
	object5 = new cMultiMesh();

	// add object to world
	world->addChild(object5);
	// set the position of the object at the center of the world
	object5->setLocalPos(0.05, 0.07, 0.09);


	// rotate the object 90 degrees
	object5->rotateAboutGlobalAxisDeg(cVector3d(-0.5, -0.5, -1), 100);

	// load an object file
	bool fileload5;
	fileload5 = object5->loadFromFile(RESOURCE_PATH("../resources/models/AC Cobra/ShelbyWD.3ds"));
	if (!fileload)
	{
#if defined(_MSVC)
		fileload5= object5->loadFromFile("../../../bin/resources/models/AC Cobra/ShelbyWD.3ds");
#endif
	}
	if (!fileload5)
	{
		cout << "Error - 3D Model failed to load correctly." << endl;
		close();
		return (-1);
	}

	// get dimensions of object
	object5->computeBoundaryBox(true);
	double size5 = cSub(object4->getBoundaryMax(), object4->getBoundaryMin()).length();

	// resize object to screen
	if (size5 > 0.001)
	{
		object5->scale(1 / size3);
	}

	// compute collision detection algorithm
	object5->createAABBCollisionDetector(.02);

	// disable culling so that faces are rendered on both sides
	object5->setUseCulling(false);

	// enable display list for faster graphic rendering
	object5->setUseDisplayList(true);

	// center object in scene
	object5->setLocalPos(-1.0 * object4->getBoundaryCenter());

	// rotate object in scene
	object5->rotateExtrinsicEulerAnglesDeg(0, 0, 90, C_EULER_ORDER_XYZ);

	// set haptic properties
	cMaterial mat5;
	mat5.setHapticTriangleSides(true, true);
	mat5.setStiffness(0.2 * maxStiffness2);
	mat5.setStaticFriction(0.2);
	mat5.setDynamicFriction(0.1);
	object5->setMaterial(mat5);


	//--------------------------------------------------------------------------
	// CREATE SHADERS
	//--------------------------------------------------------------------------

	// create vertex shader
	cShaderPtr vertexShader = cShader::create(C_VERTEX_SHADER);

	// load vertex shader from file
	fileload = vertexShader->loadSourceFile("../resources/shaders/bump.vert");
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = vertexShader->loadSourceFile("../../../bin/resources/shaders/bump.vert");
#endif
	}

	// create fragment shader
	cShaderPtr fragmentShader = cShader::create(C_FRAGMENT_SHADER);

	// load fragment shader from file
	fileload = fragmentShader->loadSourceFile("../resources/shaders/bump.frag");
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = fragmentShader->loadSourceFile("../../../bin/resources/shaders/bump.frag");
#endif
	}

	// create program shader
	cShaderProgramPtr programShader = cShaderProgram::create();

	// assign vertex shader to program shader
	programShader->attachShader(vertexShader);

	// assign fragment shader to program shader
	programShader->attachShader(fragmentShader);

	// assign program shader to object
	object->setShaderProgram(programShader);

	// link program shader
	programShader->linkProgram();

	// set uniforms
	programShader->setUniformi("uColorMap", 0);
	programShader->setUniformi("uShadowMap", 0);
	programShader->setUniformi("uNormalMap", 2);
	programShader->setUniformf("uInvRadius", 0.0f);

	// insert 7 fixed cubes
	if (new_object(argc, argv, cVector3d(-1.5, -1.5, 0.0), 0) == -1) {
		cout << "Error - New object could not be created." << endl;
	}
	if (new_object(argc, argv, cVector3d(-1.5, -1.0, 0.0), 1) == -1) {
		cout << "Error - New object could not be created." << endl;
	}
	if (new_object(argc, argv, cVector3d(-1.5, -0.5, 0.0), 2) == -1) {
		cout << "Error - New object could not be created." << endl;
	}
	if (new_object(argc, argv, cVector3d(-1.5, 0.0, 0.0), 3) == -1) {
		cout << "Error - New object could not be created." << endl;
	}
	if (new_object(argc, argv, cVector3d(-1.5, 0.5, 0.0), 4) == -1) {
		cout << "Error - New object could not be created." << endl;
	}
	if (new_object(argc, argv, cVector3d(-1.5, 1.0, 0.0), 5) == -1) {
		cout << "Error - New object could not be created." << endl;
	}
	if (new_object(argc, argv, cVector3d(-1.5, 1.5, 0.0), 6) == -1) {
		cout << "Error - New object could not be created." << endl;
	}

	// insert a wall
	if (new_wall(argc, argv) == -1) {
		cout << "Error - New wall could not be created." << endl;
	}
	
	//--------------------------------------------------------------------------
	// CREATE ENVIRONMENT GLOBE
	//--------------------------------------------------------------------------

#if 1

	// create a virtual mesh
	cMesh* floor = new cMesh();
	cMesh* wall = new cMesh();

	// add object to world
	world->addChild(floor);
	world->addChild(wall);

	// set the position of the object at the center of the world
	floor->setLocalPos(0.0, 0.0, -0.3);
	wall->setLocalPos(0.0, 0.0, 1.6);

	// create room
	cCreatePlane(floor, 6, 4);
	floor->setUseDisplayList(true);

	cCreateBox(wall, 6, 4, 4);
	wall->setUseDisplayList(true);

	// create a texture
	cTexture2dPtr textureFloor = cTexture2d::create();
	cTexture2dPtr textureWall = cTexture2d::create();

	fileload = textureFloor->loadFromFile("./resources/images/sand-wall.png");
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = textureFloor->loadFromFile("./resources/images/sand-wall.png");
#endif
	}
	bool fileload3 = textureWall->loadFromFile("./resources/images/white-wall.png");
	if (!fileload2)
	{
#if defined(_MSVC)
		fileload3 = textureWall->loadFromFile("./resources/images/white-wall.png");
#endif
	}
	if (!(fileload && fileload3))
	{
		cout << "Error - Texture image failed to load correctly." << endl;
		close();
		return (-1);
	}

	// apply texture to object
	floor->setTexture(textureFloor);
	wall->setTexture(textureWall);

	// enable texture rendering 
	floor->setUseTexture(true);
	wall->setUseTexture(true);


	// Since we don't need to see our polygons from both sides, we enable culling.
	floor->setUseCulling(false);
	wall->setUseCulling(false);

	// disable material properties and lighting
	floor->setUseMaterial(false);
	wall->setUseMaterial(false);

#else

	// create a virtual mesh
	cMesh* globe = new cMesh();

	// add object to world
	world->addChild(globe);

	// set the position of the object at the center of the world
	globe->setLocalPos(0.0, 0.0, 0.0);

	// create cube
	cCreateSphere(globe, 6.0, 360, 360);
	globe->setUseDisplayList(true);
	globe->deleteCollisionDetector();

	// create a texture
	cTexture2dPtr textureSpace = cTexture2d::create();

	fileload = textureSpace->loadFromFile(RESOURCE_PATH("../resources/images/infinity.jpg"));
	if (!fileload)
	{
	#if defined(_MSVC)
		fileload = textureSpace->loadFromFile("../../../bin/resources/images/infinity.jpg");
	#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
		close();
		return (-1);
	}

	// apply texture to object
	globe->setTexture(textureSpace);

	// enable texture rendering 
	globe->setUseTexture(true);

	// Since we don't need to see our polygons from both sides, we enable culling.
	globe->setUseCulling(false);

	// disable material properties and lighting
	globe->setUseMaterial(false);

#endif


	//--------------------------------------------------------------------------
	// START SIMULATION
	//--------------------------------------------------------------------------

	// create a thread which starts the main haptics rendering loop
	cThread* hapticsThread = new cThread();
	hapticsThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);

	// setup callback when application exits
	atexit(close);


	//--------------------------------------------------------------------------
	// MAIN GRAPHIC LOOP
	//--------------------------------------------------------------------------

	while (!simulationFinished)
	{
		// handle key presses
		processEvents();

		// react to key input
		computeMatricesFromInput();

		// check if Oculus should be used
		if (useOculus)
		{
			// start rendering
			oculusVR.onRenderStart();
		}

		// render frame for each eye
		for (int eyeIndex = 0; eyeIndex < ovrEye_Count; eyeIndex++)
		{
			// retrieve projection and modelview matrix from oculus
			cTransform projectionMatrix, modelViewMatrix;
			// check if Oculus should be used
			if (useOculus)
			{
				oculusVR.onEyeRender(eyeIndex, projectionMatrix, modelViewMatrix);
			}

			// check if Oculus should be used
			camera->m_useCustomProjectionMatrix = useOculus;

			// check if Oculus should be used
			if (useOculus)
			{
				camera->m_projectionMatrix = projectionMatrix;
			}

			// check if Oculus should be used
			//camera->m_useCustomModelViewMatrix = true;
			camera->m_useCustomModelViewMatrix = useOculus;
			
			// check if Oculus should be used
			if (useOculus)
			{
				camera->m_modelViewMatrix = modelViewMatrix;
			}

			// check if Oculus should be used
			if (useOculus)
			{
				// render world
				ovrSizei size = oculusVR.getEyeTextureSize(eyeIndex);
				camera->renderView(size.w, size.h, 0, C_STEREO_LEFT_EYE, false);
			}
			else
				camera->renderView(1000, 1000, 0, C_STEREO_LEFT_EYE, false);

			// check if Oculus should be used
			if (useOculus)
			{
				// finalize rendering
				oculusVR.onEyeRenderFinish(eyeIndex);
			}
		}

		// check if Oculus should be used
		if (useOculus)
		{
			// update frames
			oculusVR.submitFrame();
			oculusVR.blitMirror();
		}
		SDL_GL_SwapWindow(renderContext.window);
	}

	// check if Oculus should be used
	if (useOculus)
	{
		// cleanup
		oculusVR.destroyVR();
		renderContext.destroy();
	}

	// exit SDL
	SDL_Quit();

	return (0);
}

//------------------------------------------------------------------------------

void processEvents()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				close();
				break;
			}
			// spacebar
			/*
			if (event.key.keysym.sym == SDLK_SPACE)
			{
				oculusVR.recenterPose();
				break;
			}
			*/
			if (event.key.keysym.sym == SDLK_SPACE)
			{
				keyState[(unsigned char)' '] = 1;
			}
			if (event.key.keysym.sym == SDLK_1)
			{
				keyState[(unsigned char)'1'] = 1;
			}
			if (event.key.keysym.sym == SDLK_2)
			{
				keyState[(unsigned char)'2'] = 1;
			}
			if (event.key.keysym.sym == SDLK_w)
			{
				keyState[(unsigned char)'w'] = 1;
			}
			if (event.key.keysym.sym == SDLK_s)
			{
				keyState[(unsigned char)'s'] = 1;
			}
			if (event.key.keysym.sym == SDLK_a)
			{
				keyState[(unsigned char)'a'] = 1;
			}
			if (event.key.keysym.sym == SDLK_d)
			{
				keyState[(unsigned char)'d'] = 1;
			}
			if (event.key.keysym.sym == SDLK_q)
			{
				keyState[(unsigned char)'q'] = 1;
			}
			if (event.key.keysym.sym == SDLK_e)
			{
				keyState[(unsigned char)'e'] = 1;
			}

			break;

		case SDL_KEYUP:

			if (event.key.keysym.sym == SDLK_SPACE)
			{
				keyState[(unsigned char)' '] = 0;
			}
			if (event.key.keysym.sym == SDLK_1)
			{
				keyState[(unsigned char)'1'] = 0;
			}
			if (event.key.keysym.sym == SDLK_2)
			{
				keyState[(unsigned char)'2'] = 0;
			}
			if (event.key.keysym.sym == SDLK_w)
			{
				keyState[(unsigned char)'w'] = 0;
			}
			if (event.key.keysym.sym == SDLK_s)
			{
				keyState[(unsigned char)'s'] = 0;
			}
			if (event.key.keysym.sym == SDLK_a)
			{
				keyState[(unsigned char)'a'] = 0;
			}
			if (event.key.keysym.sym == SDLK_d)
			{
				keyState[(unsigned char)'d'] = 0;
			}
			if (event.key.keysym.sym == SDLK_q)
			{
				keyState[(unsigned char)'q'] = 0;
			}
			if (event.key.keysym.sym == SDLK_e)
			{
				keyState[(unsigned char)'e'] = 0;
			}

			break;

		case SDL_QUIT:
			close();
			break;

		default:
			break;
		}
	}
}

//------------------------------------------------------------------------------

void processEvents1()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			// esc
			if (event.key.keysym.sym == SDLK_q || event.key.keysym.sym == SDLK_ESCAPE)
			{
				close();
				break;
			}

			// spacebar
			if (event.key.keysym.sym == SDLK_SPACE)
			{
				oculusVR.recenterPose();
				break;
			}

			break;

		case SDL_QUIT:
			close();
			break;

		default:
			break;
		}
	}
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

void computeMatricesFromInput()
{
	if (keyState[(unsigned char)'w'] == 1) // Walk forward
	{
		currentPosition += speed * currentDirection;
	}
	if (keyState[(unsigned char)'s'] == 1) // Walk backward
	{
		currentPosition -= speed * currentDirection;
	}
	if (keyState[(unsigned char)'a'] == 1) // Turn left
	{
		currentAngle += rotationalSpeed;
	}
	if (keyState[(unsigned char)'d'] == 1) // Turn right
	{
		currentAngle -= rotationalSpeed;
	}
	if (keyState[(unsigned char)' '] == 1) // Recenter
	{
		currentPosition = INITIAL_POSITION;
		currentAngle = 0;
	}
	if (keyState[(unsigned char)'q'] == 1) // raise
	{
		currentPosition += speed * cVector3d(0.0, 0.0, 1.0);
	}
	if (keyState[(unsigned char)'e'] == 1) // lower
	{
		currentPosition -= speed * cVector3d(0.0, 0.0, 1.0);
	}
	if (keyState[(unsigned char)'1'] == 1) // special function 1
	{
		delete_object();
	}
	if (keyState[(unsigned char)'2'] == 1) // special function 2
	{
		//delete_object()
	}

	// make sure that it is not possible to walk out of the room
	if (currentPosition.x() > 2.5) {
		currentPosition.x(2.5);
	}
	if (currentPosition.x() < -2.5) {
		currentPosition.x(-2.5);
	}
	if (currentPosition.y() > 1.5) {
		currentPosition.y(1.5);
	}
	if (currentPosition.y() < -1.5) {
		currentPosition.y(-1.5);
	}
	if (currentPosition.z() > 3.0) {
		currentPosition.z(3.0);
	}
	if (currentPosition.z() < 0.0) {
		currentPosition.z(0.0);
	}

	// recalculate the viewing direction
	currentDirection = cVector3d(-cos(currentAngle), -sin(currentAngle), 0.0);

	// recalculate the direction of the "up" vector
	camera->set(currentPosition, currentPosition + currentDirection, cVector3d(0, 0, 1));   
}
//------------------------------------------------------------------------------

void close(void)
{
	// stop the simulation
	simulationRunning = false;

	// wait for graphics and haptics loops to terminate
	while (!simulationFinished) { cSleepMs(100); }

	// close haptic device
	tool->stop();
}

//------------------------------------------------------------------------------

void updateHaptics(void)
{
	// angular velocity of object
	cVector3d angVel(0.0, 0.2, 0.3);

	// reset clock
	cPrecisionClock clock;
	clock.reset();

	// simulation in now running
	simulationRunning = true;
	simulationFinished = false;

	// main haptic simulation loop
	while (simulationRunning)
	{
		/////////////////////////////////////////////////////////////////////
		// SIMULATION TIME
		/////////////////////////////////////////////////////////////////////

		// stop the simulation clock
		clock.stop();

		// read the time increment in seconds
		double timeInterval = clock.getCurrentTimeSeconds();

		// restart the simulation clock
		clock.reset();
		clock.start();

		// update frequency counter
		frequencyCounter.signal(1);


		/////////////////////////////////////////////////////////////////////
		// HAPTIC FORCE COMPUTATION
		/////////////////////////////////////////////////////////////////////

		// compute global reference frames for each object
		world->computeGlobalPositions(true);

		// update position and orientation of tool
		tool->updateFromDevice();
		cMatrix3d RotForce = cMatrix3d(cos(currentAngle), sin(currentAngle), 0.0, -sin(currentAngle), cos(currentAngle), 0.0, 0.0, 0.0, 1.0);
		cMatrix3d Rot = cMatrix3d(cos(currentAngle), -sin(currentAngle), 0.0, sin(currentAngle), cos(currentAngle), 0.0, 0.0, 0.0, 1.0);
		tool->setDeviceGlobalRot(Rot);
		cVector3d tmp = tool->getDeviceLocalPos();
		tool->setDeviceGlobalPos(Rot*tmp + currentPosition + currentDirection);

		// compute interaction forces
		tool->computeInteractionForces();

		tool->setDeviceGlobalTorque(RotForce*(tool->getDeviceGlobalTorque()));
		tool->setDeviceGlobalForce(RotForce*(tool->getDeviceGlobalForce()));

		// send forces to haptic device
		tool->applyToDevice();






		/////////////////////////////////////////////////////////////////////
		// DYNAMIC SIMULATION
		/////////////////////////////////////////////////////////////////////

		cVector3d tmp2 = tool->getDeviceGlobalPos();

		if (tmp2.x() < 0.3 && tmp2.x() > -0.3 && tmp2.y() < 0.3 && tmp2.y() > -0.3 
			&& tmp2.z() < 0.3 && tmp2.z() > -0.3)
		{
			// some constants
			const double INERTIA = 0.4;
			const double MAX_ANG_VEL = 10.0;
			const double DAMPING = 0.1;

			// get position of cursor in global coordinates
			cVector3d toolPos = tool->getDeviceGlobalPos();

			// get position of object in global coordinates
			cVector3d objectPos = object->getGlobalPos();

			// compute a vector from the center of mass of the object (point of rotation) to the tool
			cVector3d v = cSub(toolPos, objectPos);

			// compute angular acceleration based on the interaction forces
			// between the tool and the object
			cVector3d angAcc(0, 0, 0);
			if (v.length() > 0.0)
			{
				// get the last force applied to the cursor in global coordinates
				// we negate the result to obtain the opposite force that is applied on the
				// object
				cVector3d toolForce = -tool->getDeviceGlobalForce();

				// compute the effective force that contributes to rotating the object.
				cVector3d force = toolForce - cProject(toolForce, v);

				// compute the resulting torque
				cVector3d torque = cMul(v.length(), cCross(cNormalize(v), force));

				// update rotational acceleration
				angAcc = (1.0 / INERTIA) * torque;
			}

			// update rotational velocity
			angVel.add(timeInterval * angAcc);

			// set a threshold on the rotational velocity term
			double vel = angVel.length();
			if (vel > MAX_ANG_VEL)
			{
				angVel.mul(MAX_ANG_VEL / vel);
			}

			// add some damping too
			angVel.mul(1.0 - DAMPING * timeInterval);

			// if user switch is pressed, set velocity to zero
			if (tool->getUserSwitch(0) == 1)
			{
				angVel.zero();
			}

			// compute the next rotation configuration of the object
			if (angVel.length() > C_SMALL)
			{
				object->rotateAboutGlobalAxisRad(cNormalize(angVel), timeInterval * angVel.length());
			}
		}
		else
		{
			angVel.zero();
		}
	}

	// exit haptics thread
	simulationFinished = true;
}

//------------------------------------------------------------------------------

int new_object(int argc, char **argv, cVector3d position, int property)
{
	// retrieve information about the current haptic device
	cHapticDeviceInfo hapticDeviceInfoX = hapticDevice->getSpecifications();

	// define the radius of the tool (sphere)
	double toolRadius = 0.02;

	// parse first arg to try and locate resources
	//string resourceRoot = string(argv[0]).substr(0, string(argv[0]).find_last_of("/\\") + 1);
	//resourcesPath = resourceRoot + string("../examples/SDL/finalproject/resources/");

	// convert to resource path
//#define RESOURCE_PATH(p)    (char*)((resourceRoot+string(p)).c_str())
//	string path;

	// --------------------------------------------------------
	// copied from 01-cube.cpp and changed names of variables
	// --------------------------------------------------------

	// read the scale factor between the physical workspace of the haptic
	// device and the virtual workspace defined for the tool
	double workspaceScaleFactor = tool->getWorkspaceScaleFactor();

	// stiffness properties
	double maxStiffness = hapticDeviceInfoX.m_maxLinearStiffness / workspaceScaleFactor;

	// create a virtual mesh
	objectX = new cMesh();

	// add object to world
	world->addChild(objectX);

	// set the position of the object at the center of the world
	objectX->setLocalPos(position);

	// create cube
	cCreateBox(objectX, 0.2, 0.2, 0.2);

	// create a texture
	cTexture2dPtr texture = cTexture2d::create();

	bool fileload;


	switch (property)
	{
	case(0) :
		fileload = texture->loadFromFile(RESOURCE_PATH("../resources/images/brick-color.png"));
		if (!fileload)
		{
#if defined(_MSVC)
			fileload = texture->loadFromFile("../../../bin/resources/images/brick-color.png");
#endif
		}
		if (!fileload)
		{
			cout << "Error - Texture image failed to load correctly." << endl;
			close();
			return (-1);
		}
		break;
	case(1) :
		fileload = texture->loadFromFile(RESOURCE_PATH("../resources/images/color1.png"));
		if (!fileload)
		{
#if defined(_MSVC)
			fileload = texture->loadFromFile("../../../bin/resources/images/color1.png");
#endif
		}
		if (!fileload)
		{
			cout << "Error - Texture image failed to load correctly." << endl;
			close();
			return (-1);
		}
		break;
	case(2) :
		fileload = texture->loadFromFile(RESOURCE_PATH("../resources/images/G1RhombAluminumMesh.JPG"));
		if (!fileload)
		{
#if defined(_MSVC)
			fileload = texture->loadFromFile("../../../bin/resources/images/G1RhombAluminumMesh.JPG");
#endif
		}
		if (!fileload)
		{
			cout << "Error - Texture image failed to load correctly." << endl;
			close();
			return (-1);
		}
		break;
	case(3) :
		fileload = texture->loadFromFile(RESOURCE_PATH("../resources/images/whitefoam.jpg"));
		if (!fileload)
		{
#if defined(_MSVC)
			fileload = texture->loadFromFile("../../../bin/resources/images/whitefoam.jpg");
#endif
		}
		if (!fileload)
		{
			cout << "Error - Texture image failed to load correctly." << endl;
			close();
			return (-1);
		}
		break;
	case(4) :
		fileload = texture->loadFromFile(RESOURCE_PATH("../resources/images/brownboard.jpg"));
		if (!fileload)
		{
#if defined(_MSVC)
			fileload = texture->loadFromFile("../../../bin/resources/images/brownboard.jpg");
#endif
		}
		if (!fileload)
		{
			cout << "Error - Texture image failed to load correctly." << endl;
			close();
			return (-1);
		}
		break;
	case(5) :
		fileload = texture->loadFromFile(RESOURCE_PATH("../resources/images/blackstone.jpg"));
		if (!fileload)
		{
#if defined(_MSVC)
			fileload = texture->loadFromFile("../../../bin/resources/images/blackstone.jpg");
#endif
		}
		if (!fileload)
		{
			cout << "Error - Texture image failed to load correctly." << endl;
			close();
			return (-1);
		}
		break;
	case(6) :
		fileload = texture->loadFromFile(RESOURCE_PATH("../resources/images/redplastic.jpg"));
		if (!fileload)
		{
#if defined(_MSVC)
			fileload = texture->loadFromFile("../../../bin/resources/images/redplastic.jpg");
#endif
		}
		if (!fileload)
		{
			cout << "Error - Texture image failed to load correctly." << endl;
			close();
			return (-1);
		}
		break;
	}

	// apply texture to object
	objectX->setTexture(texture);

	// enable texture rendering 
	objectX->setUseTexture(true);

	// Since we don't need to see our polygons from both sides, we enable culling.
	objectX->setUseCulling(true);

	// set material properties to light gray
	objectX->m_material->setWhite();

	// compute collision detection algorithm
	objectX->createAABBCollisionDetector(toolRadius);

	// define a default stiffness for the object
	objectX->m_material->setStiffness(0.3 * maxStiffness);

	// define some static friction
	objectX->m_material->setStaticFriction(0.2);

	// define some dynamic friction
	objectX->m_material->setDynamicFriction(0.2);

	// define some texture rendering
	objectX->m_material->setTextureLevel(0.1);

	// render triangles haptically on front side only
	objectX->m_material->setHapticTriangleSides(true, false);

	// create a normal texture
	cNormalMapPtr normalMap = cNormalMap::create();

	// load normal map from file
	switch (property)
	{
	case(0) :
		fileload = normalMap->loadFromFile(RESOURCE_PATH("../resources/images/brick-normal.png"));
		if (!fileload)
		{
#if defined(_MSVC)
			fileload = normalMap->loadFromFile("../../../bin/resources/images/brick-normal.png");
#endif
		}
		if (!fileload)
		{
			cout << "Error - Texture image failed to load correctly." << endl;
			close();
			return (-1);
		}
		break;
	case(1) :
		fileload = normalMap->loadFromFile(RESOURCE_PATH("../resources/images/normal1.png"));
		if (!fileload)
		{
#if defined(_MSVC)
			fileload = normalMap->loadFromFile("../../../bin/resources/images/normal1.png");
#endif
		}
		if (!fileload)
		{
			cout << "Error - Texture image failed to load correctly." << endl;
			close();
			return (-1);
		}
		break;
	case(2) :
		fileload = normalMap->loadFromFile(RESOURCE_PATH("../resources/images/G1RhombAluminumMeshNormal.png"));
		if (!fileload)
		{
#if defined(_MSVC)
			fileload = normalMap->loadFromFile("../../../bin/resources/images/G1RhombAluminumMeshNormal.png");
#endif
		}
		if (!fileload)
		{
			cout << "Error - Texture image failed to load correctly." << endl;
			close();
			return (-1);
		}
		break;
	case(3) :
		normalMap->createMap(objectX->m_texture);
		break;
	case(4) :
		normalMap->createMap(objectX->m_texture);
		break;
	case(5) :
		normalMap->createMap(objectX->m_texture);
		break;
	case(6) :
		normalMap->createMap(objectX->m_texture);
		break;
	}

	// assign normal map to object
	objectX->m_normalMap = normalMap;

	// compute surface normals
	objectX->computeAllNormals();

	// compute tangent vectors
	objectX->m_triangles->computeBTN();



	//--------------------------------------------------------------------------
	// CREATE SHADERS
	//--------------------------------------------------------------------------

	// create vertex shader
	cShaderPtr vertexShader = cShader::create(C_VERTEX_SHADER);

	// load vertex shader from file
	fileload = vertexShader->loadSourceFile("../resources/shaders/bump.vert");
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = vertexShader->loadSourceFile("../../../bin/resources/shaders/bump.vert");
#endif
	}

	// create fragment shader
	cShaderPtr fragmentShader = cShader::create(C_FRAGMENT_SHADER);

	// load fragment shader from file
	fileload = fragmentShader->loadSourceFile("../resources/shaders/bump.frag");
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = fragmentShader->loadSourceFile("../../../bin/resources/shaders/bump.frag");
#endif
	}



	// create program shader
	cShaderProgramPtr programShader = cShaderProgram::create();

	// assign vertex shader to program shader
	programShader->attachShader(vertexShader);

	// assign fragment shader to program shader
	programShader->attachShader(fragmentShader);

	// assign program shader to object
	objectX->setShaderProgram(programShader);

	// link program shader
	programShader->linkProgram();

	// set uniforms
	programShader->setUniformi("uColorMap", 0);
	programShader->setUniformi("uShadowMap", 0);
	programShader->setUniformi("uNormalMap", 2);
	programShader->setUniformf("uInvRadius", 0.0f);

	return 0;
}

//------------------------------------------------------------------------------

void delete_object()
{
	world->removeChild(objectX);
}

//------------------------------------------------------------------------------

int new_wall(int argc, char **argv)
{
	// retrieve information about the current haptic device
	cHapticDeviceInfo hapticDeviceInfoX = hapticDevice->getSpecifications();

	// define the radius of the tool (sphere)
	double toolRadius = 0.02;

	// --------------------------------------------------------
	// copied from 01-cube.cpp and changed names of variables
	// --------------------------------------------------------

	// read the scale factor between the physical workspace of the haptic
	// device and the virtual workspace defined for the tool
	double workspaceScaleFactor = tool->getWorkspaceScaleFactor();

	// stiffness properties
	double maxStiffness = hapticDeviceInfoX.m_maxLinearStiffness / workspaceScaleFactor;

	// create a virtual mesh
	objectX = new cMesh();

	// add object to world
	world->addChild(objectX);

	// set the position of the object at the center of the world
	objectX->setLocalPos(-2.9, 0.0, 1.6);

	// create cube
	cCreateBox(objectX, 0.01, 4.0, 4.0);

	// create a texture
	cTexture2dPtr texture = cTexture2d::create();

	bool fileload;


	fileload = texture->loadFromFile(RESOURCE_PATH("../../examples/SDL/haptics-showroom-V02/resources/images/stone.jpg"));

	//cout << "resourcesPath = " << resourcesPath << endl;

	//cout << "image to be loaded: " << RESOURCE_PATH("../../examples/SDL/haptics-showroom-V02/resources/images/stone.jpg") << endl;

	if (!fileload)
	{
#if defined(_MSVC)
	//fileload = texture->loadFromFile("../../../bin/resources/images/stone.jpg");
	//fileload = texture->loadFromFile("D:/Users/ga87taq/Desktop/Labs/chai3d-3.1.0/modules/OCULUS/examples/SDL/haptics-showroom-V02/resources/images/stone.jpg");
	fileload = texture->loadFromFile("./../../examples/SDL/haptics-showroom-V02/resources/images/stone.jpg");
	//fileload = texture->loadFromFile("./stone.jpg");
	cout << "Loaded from somewhere else" << endl;
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly. XXX" << endl;
		close();
		return (-1);
	}

	// apply texture to object
	objectX->setTexture(texture);

	// enable texture rendering 
	objectX->setUseTexture(true);

	// Since we don't need to see our polygons from both sides, we enable culling.
	objectX->setUseCulling(true);

	// set material properties to light gray
	objectX->m_material->setWhite();

	// compute collision detection algorithm
	objectX->createAABBCollisionDetector(toolRadius);

	// define a default stiffness for the object
	objectX->m_material->setStiffness(0.3 * maxStiffness);

	// define some static friction
	objectX->m_material->setStaticFriction(0.2);

	// define some dynamic friction
	objectX->m_material->setDynamicFriction(0.2);

	// define some texture rendering
	objectX->m_material->setTextureLevel(0.1);

	// render triangles haptically on front side only
	objectX->m_material->setHapticTriangleSides(true, false);

	// create a normal texture
	//cNormalMapPtr normalMap = cNormalMap::create();

	// load normal map from file
#if 0
	fileload = normalMap->loadFromFile(RESOURCE_PATH("../resources/images/brick-normal.png"));
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = normalMap->loadFromFile("../../../bin/resources/images/brick-normal.png");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
		close();
		return (-1);
	}
#else
	//normalMap->createMap(objectX->m_texture);
#endif

	// assign normal map to object
	//objectX->m_normalMap = normalMap;

	// compute surface normals
	//objectX->computeAllNormals();

	// compute tangent vectors
	//objectX->m_triangles->computeBTN();



	//--------------------------------------------------------------------------
	// CREATE SHADERS
	//--------------------------------------------------------------------------

	// create vertex shader
	cShaderPtr vertexShader = cShader::create(C_VERTEX_SHADER);

	// load vertex shader from file
	fileload = vertexShader->loadSourceFile("../resources/shaders/bump.vert");
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = vertexShader->loadSourceFile("../../../bin/resources/shaders/bump.vert");
#endif
	}

	// create fragment shader
	cShaderPtr fragmentShader = cShader::create(C_FRAGMENT_SHADER);

	// load fragment shader from file
	fileload = fragmentShader->loadSourceFile("../resources/shaders/bump.frag");
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = fragmentShader->loadSourceFile("../../../bin/resources/shaders/bump.frag");
#endif
	}



	// create program shader
	cShaderProgramPtr programShader = cShaderProgram::create();

	// assign vertex shader to program shader
	programShader->attachShader(vertexShader);

	// assign fragment shader to program shader
	programShader->attachShader(fragmentShader);

	// assign program shader to object
	objectX->setShaderProgram(programShader);

	// link program shader
	programShader->linkProgram();

	// set uniforms
	programShader->setUniformi("uColorMap", 0);
	programShader->setUniformi("uShadowMap", 0);
	programShader->setUniformi("uNormalMap", 2);
	programShader->setUniformf("uInvRadius", 0.0f);

	return 0;
}