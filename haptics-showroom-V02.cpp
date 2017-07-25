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
#include "Global.h"
#include "MyObjectDatabase.h"
//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;
//------------------------------------------------------------------------------




////////////////////////////////////////
// define if Oculus Rift is used or not
bool useOculus = false;
////////////////////////////////////////

////////////////////////////////////////
// define if two haptic devices are used
bool useSecondHapticDevice = false;
////////////////////////////////////////




//------------------------------------------------------------------------------
// DECLARED MACROS
//------------------------------------------------------------------------------

// convert to resource path
//#define RESOURCE_PATH(p)    (char*)((resourcesPath+string(p)).c_str())	// -> defined in Global.h
#define WINDOW_SIZE_W		1000
#define WINDOW_SIZE_H		1000
#define TOOL_RADIUS			0.02
#define TOOL_WORKSPACE		0.3
//#define INITIAL_POSITION	cVector3d(2.0, 0.0, 0.3)						// -> defined in Global.h

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




// TESTING
vector<cMesh*> objectY(10);




// audio device to play sound
cAudioDevice* audioDevice;

// audio buffers to store sound files
cAudioBuffer* audioBuffer1;

// audio source of an object
cAudioSource* audioSourceObject;

// indicates if the haptic simulation currently running
bool simulationRunning = false;

// indicates if the haptic simulation has terminated
bool simulationFinished = false;

// frequency counter to measure the simulation haptic rate
cFrequencyCounter frequencyCounter;

//------------------------------------------------------------------------------
// Custum variables
//------------------------------------------------------------------------------

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
double speed = 0.006;
double rotationalSpeed = 0.006;

// distances to walls and floor (& ceiling)
const double wallDistance = 0.75;
const double floorDistance = 0.3;

// variable to store state of keys
unsigned int keyState[255];

// variable to check success of file load
bool fileload;


// ############# TESTING ############
// pointer to MyObject class
//MyObject* obj_test[10];

// pointer to MyProperties class
//MyProperties* prop_test[10];
// ############# TESTING ############


// information about the current haptic device -> retrieved at runtime
cHapticDeviceInfo hapticDeviceInfoX;

// scale factor between the physical workspace of the haptic device and 
// the virtual workspace defined for the tool -> retrieved at runtime

double workspaceScaleFactor;

// max stiffness -> retrieved at runtime
double maxStiffness;

//------------------------------------------------------------------------------
// OCULUS RIFT
//------------------------------------------------------------------------------

// display context
cOVRRenderContext renderContext;

// oculus device
cOVRDevice oculusVR;

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

// function to check if boundaries of room are violated when walking
void checkBoundaries();

// function which draws kartesian coordinates at some position
void draw_coordinates(cVector3d position, double length, double width);

// ############################# TESTING ###################################

// function to create a new object at runtime
int new_object_OLD(cVector3d position, cVector3d size, int property);

// function to create a new object at runtime with properties
int new_object_with_properties(cVector3d position, cVector3d size, MyProperties *property);

int new_plane(cVector3d position, MyProperties properties);

void new_object(cVector3d position, MyProperties properties);

// ############################# TESTING ###################################


//==============================================================================
// MAIN FUNCTION
//==============================================================================

int main(int argc, char **argv)
{
	//--------------------------------------------------------------------------
	// INITIALIZATION
	//--------------------------------------------------------------------------

	cout << endl;
	cout << "--------------------------------------------------" << endl;
	cout << "CHAI3D" << endl;
	cout << "Project: Haptics Showroom" << endl;
	cout << "Team:    Naina Dhingra, Ke Xu, Hannes Bohnengel" << endl;
	cout << "Rev.:    0.2" << endl;
	cout << "--------------------------------------------------" << endl << endl << endl;
	cout << "Keyboard Options:" << endl << endl;
	cout << "Space  - Recenter view point" << endl;
	cout << "Escape - Exit application" << endl;
	cout << "[a]    - Turn left" << endl;
	cout << "[d]    - Turn right" << endl;
	cout << "[w]    - Move forward" << endl;
	cout << "[s]    - Move backwards" << endl;
	cout << "[1]    - Hot test" << endl;
	cout << "[2]    - Cold test" << endl;
	cout << "[q]    - Raise" << endl;
	cout << "[e]    - Lower" << endl;
	cout << endl << endl;

	// get the location of the executable
	resourceRoot = string(argv[0]).substr(0, string(argv[0]).find_last_of("/\\") + 1);

	// this is the location of the resources
	resourcesPath = resourceRoot + string("../../examples/SDL/haptics-showroom-V02/resources/");

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
		renderContext.init("CHAI3D", 100, 100, windowSize.w, windowSize.h);
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

	// retrieve information about the current haptic device
	cHapticDeviceInfo hapticDeviceInfo = hapticDevice->getSpecifications();

	// create a 3D tool and add it to the world
	tool = new cToolCursor(world);
	world->addChild(tool);

	// connect the haptic device to the tool
	tool->setHapticDevice(hapticDevice);

	// define a radius for the tool
	tool->setRadius(TOOL_RADIUS);

	// enable if objects in the scene are going to rotate of translate
	// or possibly collide against the tool. If the environment
	// is entirely static, you can set this parameter to "false"
	tool->enableDynamicObjects(true);

	// map the physical workspace of the haptic device to a larger virtual workspace.
	tool->setWorkspaceRadius(TOOL_WORKSPACE);

	// haptic forces are enabled only if small forces are first sent to the device;
	// this mode avoids the force spike that occurs when the application starts when 
	// the tool is located inside an object for instance. 
	tool->setWaitForSmallForce(true);

	// start the haptic tool
	tool->start();

	// only if second device should be used
	if (useSecondHapticDevice == true)
	{
		// get access to the first available haptic device
		handler->getDevice(hapticDevice2, 1);

		// retrieve information about the current haptic device
		cHapticDeviceInfo hapticDeviceInfo2 = hapticDevice2->getSpecifications();

		// create a 3D tool and add it to the world
		tool2 = new cToolCursor(world);
		world->addChild(tool2);

		// connect the haptic device to the tool
		tool2->setHapticDevice(hapticDevice2);

		// define a radius for the tool
		tool->setRadius(TOOL_RADIUS);
		tool2->setRadius(TOOL_RADIUS);

		// enable if objects in the scene are going to rotate of translate
		// or possibly collide against the tool. If the environment
		// is entirely static, you can set this parameter to "false"
		tool2->enableDynamicObjects(true);

		// map the physical workspace of the haptic device to a larger virtual workspace.
		tool2->setWorkspaceRadius(TOOL_WORKSPACE);

		// haptic forces are enabled only if small forces are first sent to the device;
		// this mode avoids the force spike that occurs when the application starts when 
		// the tool is located inside an object for instance. 
		tool2->setWaitForSmallForce(true);

		// start the haptic tool
		tool2->start();
	}

	// retrieve information about the current haptic device
	hapticDeviceInfoX = hapticDevice->getSpecifications();

	// read the scale factor between the physical workspace of the haptic
	// device and the virtual workspace defined for the tool
	workspaceScaleFactor = tool->getWorkspaceScaleFactor();

	// retrieve max stiffness
	maxStiffness = hapticDeviceInfoX.m_maxLinearStiffness / workspaceScaleFactor;

	//--------------------------------------------------------------------------
	// CREATE OBJECT
	//--------------------------------------------------------------------------

	// rotating cube from 01-cube.cpp
#if 0

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
	object->setLocalPos(0.0, 0.0, 0.0);

	// create cube
	cCreateBox(object, 0.2, 0.2, 0.2);

	// create a texture
	cTexture2dPtr texture = cTexture2d::create();

	fileload = texture->loadFromFile(RESOURCE_PATH("images/brick-color.png"));
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
	fileload = normalMap->loadFromFile(RESOURCE_PATH("images/brick-normal.png"));
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

#endif

	new_object(cVector3d(-1.0, -0.5, 0.15), Cube_Aluminium);

	new_object(cVector3d(0.0, -0.5, 0.15), Cube_Aluminium);

	new_object(cVector3d(-1.0, 0.5, 0.2), Sphere_Steel);

	new_object(cVector3d(-1.0, 1.5, 0.0), Cylinder_Granite);

	new_object(cVector3d(1.0, 1.0, 0.2), Cube_WoodProfiled);

//	new_object_new(objectY, cVector3d(1.0, 1.0, 0.2), Cube_WoodProfiled);

	//--------------------------------------------------------------------------
	// CREATE ROOM
	//--------------------------------------------------------------------------

	// draw a coordinate system for easier orientation
	draw_coordinates(cVector3d(-0.5, -0.5, 0.05), 0.3, 1.0);
	
	// floor
	new_plane(cVector3d(0.0, 0.0, 0.0), myFloor);
	
	// ceiling
	new_plane(cVector3d(0.0, 0.0, roomHeight), myCeiling);

	// right wall
	new_plane(cVector3d(0.0, (roomWidth / 2), (roomHeight / 2)), myRightWall);

	// left wall
	new_plane(cVector3d(0.0, -(roomWidth / 2), (roomHeight / 2)), myLeftWall);
	
	// back wall
	new_plane(cVector3d(-(roomLength / 2), 0.0, (roomHeight / 2)), myBackWall);

	// front wall
	new_plane(cVector3d((roomLength / 2), 0.0, (roomHeight / 2)), myFrontWall);

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

		// avoid walking out of room
		checkBoundaries();

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
		tool->setDeviceGlobalPos(currentPosition);
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
		//delete_object();
	}
	if (keyState[(unsigned char)'2'] == 1) // special function 2
	{
		//delete_object()
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

		if (useSecondHapticDevice == true)
		{
			// ############################################################################################




			// => Insert haptics stuff for second device




			// ############################################################################################
		}


#if 0

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
		// END DYNAMIC SIMULATION

#endif
	}

	// exit haptics thread
	simulationFinished = true;
}

//------------------------------------------------------------------------------

void checkBoundaries()
{
	// make sure that it is not possible to walk out of the room
	if (currentPosition.x() > ((roomLength / 2) - wallDistance)) {
		currentPosition.x(((roomLength / 2) - wallDistance));
	}
	if (currentPosition.x() < -((roomLength / 2) - wallDistance)) {
		currentPosition.x(-((roomLength / 2) - wallDistance));
	}
	if (currentPosition.y() > ((roomWidth / 2) - wallDistance)) {
		currentPosition.y((roomWidth / 2) - wallDistance);
	}
	if (currentPosition.y() < -((roomWidth / 2) - wallDistance)) {
		currentPosition.y(-((roomWidth / 2) - wallDistance));
	}
	if (currentPosition.z() > ((roomHeight)-floorDistance)) {
		currentPosition.z((roomHeight)-floorDistance);
	}
	if (currentPosition.z() < floorDistance) {
		currentPosition.z(floorDistance);
	}
}

//------------------------------------------------------------------------------

void draw_coordinates(cVector3d position, double length, double width)
{

	// ------------------
	// x-axis 
	// ------------------

	cVector3d temp(length, 0.0, 0.0);

	// create small line to illustrate the velocity of the haptic device
	cShapeLine* x_axis = new cShapeLine(position, cVector3d(position + temp));

	// set line width of axis
	x_axis->setLineWidth(width);

	// set the color of the axis
	x_axis->m_colorPointA.setRed();
	x_axis->m_colorPointB.setRed();

	// insert line inside world
	world->addChild(x_axis);

	// ------------------
	// y-axis 
	// ------------------

	temp.x(0);
	temp.y(length);

	// create small line to illustrate the velocity of the haptic device
	cShapeLine* y_axis = new cShapeLine(position, cVector3d(position + temp));

	// set line width of axis
	y_axis->setLineWidth(width);

	// set the color of the axis
	y_axis->m_colorPointA.setGreen();
	y_axis->m_colorPointB.setGreen();

	// insert line inside world
	world->addChild(y_axis);

	// ------------------
	// z-axis 
	// ------------------

	temp.y(0);
	temp.z(length);

	// create small line to illustrate the velocity of the haptic device
	cShapeLine* z_axis = new cShapeLine(cVector3d(position), cVector3d(position + temp));

	// set line width of axis
	z_axis->setLineWidth(width);

	// set the color of the axis
	z_axis->m_colorPointA.setBlue();
	z_axis->m_colorPointB.setBlue();

	// insert line inside world
	world->addChild(z_axis);
}

//------------------------------------------------------------------------------

void new_object(cVector3d position, MyProperties properties)
{
	cout << "Creating new object " << endl;

	// create a virtual mesh
	objectX = new cMesh();

	// add object to world
	world->addChild(objectX);

	// set the position of the object at the center of the world
	objectX->setLocalPos(position);

	switch (properties.shape)
	{
	case(cube) :
		// create cube
		chai3d::cCreateBox(objectX, properties.size.x(), properties.size.y(), properties.size.z());
		break;
	case (sphere) :
		// create sphere
		chai3d::cCreateSphere(objectX, (const double)properties.size.length() / 2.5);
		break;
	case(cylinder) :
		chai3d::cCreateCylinder(objectX, (const double)properties.size.z(), cVector3d(properties.size.x(), properties.size.y(), 0.0).length() / 2);
		break;

/*	case(complex3ds) :
		break;
*/
	}

	// create a texture
	cTexture2dPtr texture = cTexture2d::create();

	// load texture image from file
	if (texture->loadFromFile(RESOURCE_PATH(STR_ADD("images/", properties.textureImage))) != 1)
	{
		cout << "ERROR: Cannot load texture file!" << endl;
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
	objectX->createAABBCollisionDetector(TOOL_RADIUS);

	// define a default stiffness for the object
	objectX->m_material->setStiffness(properties.stiffness * maxStiffness);

	// define some static friction
	objectX->m_material->setStaticFriction(properties.staticFriction);

	// define some dynamic friction
	objectX->m_material->setDynamicFriction(properties.dynamicFriction);

	// define some texture rendering
	objectX->m_material->setTextureLevel(properties.textureLevel);

	// render triangles haptically on front side only
	objectX->m_material->setHapticTriangleSides(true, false);

	// create a normal texture
	cNormalMapPtr normalMap = cNormalMap::create();

	// load normal map from file
	if (normalMap->loadFromFile(RESOURCE_PATH(STR_ADD("images/", properties.normalImage))) != 1)
	{
		cout << "ERROR: Cannot load normal map file!" << endl;
		normalMap->createMap(objectX->m_texture);
	}

	// assign normal map to object
	objectX->m_normalMap = normalMap;

	// compute surface normals
	objectX->computeAllNormals();


	// #################################################################
	// THIS RISES PROBLEMS FOR SHPERES !!!

	// compute tangent vectors
	
	if (properties.shape != sphere)
		objectX->m_triangles->computeBTN();

	// #################################################################

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


	//--------------------------------------------------------------------------
	// SETUP AUDIO MATERIAL
	//--------------------------------------------------------------------------

	// check if audio gain is bigger than zero
	if (properties.audioGain > 0.0f)
	{
		// create an audio device to play sounds
		audioDevice = new cAudioDevice();

		// attach audio device to camera
		camera->attachAudioDevice(audioDevice);

		// create an audio buffer and load audio wave file
		audioBuffer1 = audioDevice->newAudioBuffer();

		if (audioBuffer1->loadFromFile(RESOURCE_PATH((STR_ADD("sounds/", properties.audio)))) != 1)
		{
			cout << "ERROR: Cannot load audio file!" << endl;
		}

		// here we convert all files to mono. this allows for 3D sound support. if this code
		// is commented files are kept in stereo format and 3D sound is disabled. Compare both!
		audioBuffer1->convertToMono();

		// create an audio source for this tool.
		tool->createAudioSource(audioDevice);

		// assign auio buffer to audio source
		//audioSourceObject->setAudioBuffer(audioBuffer1);

		// loop playing of sound
		//audioSourceObject->setLoop(true);

		// turn off sound for now
		//audioSourceObject->setGain(0.0);

		// set pitch
		//audioSourceObject->setPitch(0.2);

		// play sound
		//audioSourceObject->play();
		
		// set audio properties
		objectX->m_material->setAudioImpactBuffer(audioBuffer1);
		objectX->m_material->setAudioFrictionBuffer(audioBuffer1);
		objectX->m_material->setAudioFrictionGain((const double)properties.audioGain);
		objectX->m_material->setAudioFrictionPitchGain((const double)properties.audioPitchGain);
		objectX->m_material->setAudioFrictionPitchOffset((const double)properties.audioPitchOffset);
	}
}

//------------------------------------------------------------------------------

int new_plane(cVector3d position, MyProperties properties){

	// create a virtual mesh
	cMesh* plane = new cMesh();

	// add object to world
	world->addChild(plane);

	// set the position of the object at the center of the world
	plane->setLocalPos(position.x(), position.y(), position.z());

	// create shape
	cCreatePlane(plane, properties.size.x(), properties.size.y());
	plane->setUseDisplayList(true);

	// create collision detector
	plane->createAABBCollisionDetector(TOOL_RADIUS);

	// create a texture
	cTexture2dPtr textureFloor = cTexture2d::create();

	//"./resources/images/sand-wall.png"
	if (textureFloor->loadFromFile(RESOURCE_PATH(STR_ADD("images/", properties.textureImage))) != 1)
	{
		cout << "ERROR: Cannot load texture file!" << endl;
	}

	// apply texture to object
	plane->setTexture(textureFloor);

	// create normal map from texture data
	cNormalMapPtr normalMap1 = cNormalMap::create();
	normalMap1->createMap(plane->m_texture);
	plane->m_normalMap = normalMap1;

	// enable texture rendering 
	plane->setUseTexture(true);

	// Since we don't need to see our polygons from both sides, we enable culling.
	plane->setUseCulling(false);

	// disable material properties and lighting
	plane->setUseMaterial(false);

	// set material properties to light gray
	plane->m_material->setWhite();

	// set haptic properties
	plane->m_material->setStiffness(properties.stiffness* maxStiffness);
	plane->m_material->setStaticFriction(properties.staticFriction);
	plane->m_material->setDynamicFriction(properties.dynamicFriction);
	plane->m_material->setTextureLevel(properties.textureLevel);
	plane->m_material->setHapticTriangleSides(true, false);

	// set the orientation
	plane->rotateAboutLocalAxisDeg(properties.orientation.axis, properties.orientation.rotation);

	return 0;
}

