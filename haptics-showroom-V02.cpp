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
#include "MySerial2Arduino.h"
#include "MyRegions.h"
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

#define WINDOW_SIZE_W		1000
#define WINDOW_SIZE_H		1000
#define TOOL_RADIUS			0.02
#define TOOL_WORKSPACE		0.3

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

// a vector containing the virtual object pointers
vector<cMesh*> object(MAX_OBJECT_COUNT);

// a counter for virtual objects
int objectCounter = 0;

// for 3ds objects
vector<cMultiMesh*> object3ds(MAX_OBJECT_COUNT);

// a counter for virtual objects which use 3ds files
int object3dsCounter = 0;

// a vector with objects containing information about the temperature regions
vector<MyRegions*> tempRegion(MAX_OBJECT_COUNT);

// a counter for the temperature regions
int tempRegionCounter = 0;

// a vector for audio buffer pointers
vector<cAudioBuffer*> audioBuffer(MAX_OBJECT_COUNT);

// a counter for the audio buffers
int audioBufferCounter = 0;

// audio device to play sound
cAudioDevice* audioDevice;

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
double speed = 0.032;
double rotationalSpeed = 0.022;
//double currentAngleV = 15;

// distances to walls and floor (& ceiling)
const double wallDistance = 0.75;
const double floorDistance = 0.3;

// variable to store state of keys
unsigned int keyState[255];

// variable to check success of file load
bool fileload;

// this variable stores the current temperature status
enum MyTempStatus myTemp = standby;

// input buffer to receive data over serial
const int sizeInputBuffer = 50;
char inputBuffer[sizeInputBuffer] = { 0 };

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
void updateHapticsOLD(void);
void updateHaptics(void);

// process keyboard events
void processEvents();

// function that computes movements through key input
void computeMatricesFromInput();

// function to check if boundaries of room are violated when walking
void checkBoundaries();

// function which draws kartesian coordinates at some position
void drawCoordinates(cVector3d position, double length, double width);

// function to check wether near a hot or cold object
int checkTempRegions();

// function to create planes with enabled force feedback (walls, floor)
int new_plane(cVector3d position, MyProperties properties);

// function to create new objects based on cMesh
int new_object_cMesh(cVector3d position, MyProperties properties);

// function to create new objects based on cMultiMesh (e.g. 3ds files)
int new_object_cMultiMesh(cVector3d position, MyProperties properties, string objectFile, double scalingFactor);

//==============================================================================
// MAIN FUNCTION
//==============================================================================

int main(int argc, char **argv)
{
	//--------------------------------------------------------------------------
	// INITIALIZATION
	//--------------------------------------------------------------------------
	cout << endl;
	cout << "==========================================================" << endl;
	cout << "Project: Haptics Showroom" << endl;
	cout << "Team:    Naina Dhingra, Ke Xu, Hannes Bohnengel" << endl;
	cout << "Rev.:    0.2" << endl;
	cout << "----------------------------------------------------------" << endl;
	cout << "General Commands:" << endl << endl;
	cout << "Space  - Recenter view point" << endl;
	cout << "Escape - Exit application" << endl;
	//cout << "[4]    - Print haptic device position" << endl;
	cout << "----------------------------------------------------------" << endl;
	cout << "Moving Commands:           |     Temperature Commands:" << endl;
	cout << "                           |" << endl;
	cout << "[a]    - Turn left         |     [1] - Heating on" << endl;
	cout << "[d]    - Turn right        |     [2] - Cooling on" << endl;
	cout << "[w]    - Move forward      |     [3] - Peltier disable" << endl;
	cout << "[s]    - Move backwards    |" << endl;
	cout << "[q]    - Raise             |" << endl;
	cout << "[e]    - Lower             |" << endl;
	cout << "==========================================================" << endl << endl;

	//--------------------------------------------------------------------------
	// EXTRACT CURRENT PATH
	//--------------------------------------------------------------------------

	// get the location of the executable
	resourceRoot = string(argv[0]).substr(0, string(argv[0]).find_last_of("/\\") + 1);

	// this is the location of the resources
	resourcesPath = resourceRoot + string("../../examples/SDL/haptics-showroom-V02/resources/");

	//--------------------------------------------------------------------------
	// INIT ARDUINO
	//--------------------------------------------------------------------------

	InitSerial2Arduino();

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
	
	// create an audio device to play sounds
	audioDevice = new cAudioDevice();

	// attach audio device to camera
	camera->attachAudioDevice(audioDevice);

	// create an audio source for this tool.
	tool->createAudioSource(audioDevice);

	// start the haptic tool
	tool->start();

	// retrieve information about the current haptic device
	hapticDeviceInfoX = hapticDevice->getSpecifications();

	// read the scale factor between the physical workspace of the haptic
	// device and the virtual workspace defined for the tool
	workspaceScaleFactor = tool->getWorkspaceScaleFactor();

	// retrieve max stiffness
	maxStiffness = hapticDeviceInfoX.m_maxLinearStiffness / workspaceScaleFactor;

	//--------------------------------------------------------------------------
	// CREATE ROOM
	//--------------------------------------------------------------------------

	cout << "Creating the room." << endl;

	// draw a coordinate system for easier orientation
	//drawCoordinates(cVector3d(-0.5, -0.5, 0.05), 0.3, 1.0);

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

	// window
	new_plane(cVector3d(-(roomLength / 2), 0.0, (roomHeight / 2.5)), myWindow);

	// title
	new_plane(cVector3d(-(roomLength / 2), 0.0, (roomHeight / 1.25)), myTitle);

	//--------------------------------------------------------------------------
	// CREATE OBJECT
	//--------------------------------------------------------------------------

	cout << "Creating the objects." << endl;

	// place the objects on the tables

	// LEFT SIDE

	// Aluminium Mesh
	new_plane(cVector3d(2.0, -(roomWidth / 2), 0.75), myLeftLableAM);
	new_object_cMultiMesh(cVector3d(2.0, (-(roomWidth / 2) + 0.5), -0.005), Property_3ds, "table.3ds", 1);
	new_object_cMesh(cVector3d(2.0, (-(roomWidth / 2) + 0.5), 0.265), Cube_Aluminium);

	// Silver Oak Wood
	new_plane(cVector3d(1.0, -(roomWidth / 2), 0.75), myLeftLableSO);
	new_object_cMultiMesh(cVector3d(1.0, (-(roomWidth / 2) + 0.5), -0.005), Property_3ds, "table.3ds", 1);
	new_object_cMesh(cVector3d(1.0, (-(roomWidth / 2) + 0.5), 0.265), Cube_SilverOak);

	// Crushed Rock
	new_plane(cVector3d(0.0, -(roomWidth / 2), 0.75), myLeftLableCR);
	new_object_cMultiMesh(cVector3d(0.0, (-(roomWidth / 2) + 0.5), -0.005), Property_3ds, "table.3ds", 1);
	new_object_cMesh(cVector3d(0.0, (-(roomWidth / 2) + 0.5), 0.265), Cube_CrushedRock);

	// Stainless Steel
	new_plane(cVector3d(-1.0, -(roomWidth / 2), 0.75), myLeftLableSS);
	new_object_cMultiMesh(cVector3d(-1.0, (-(roomWidth / 2) + 0.5), -0.005), Property_3ds, "table.3ds", 1);
	new_object_cMesh(cVector3d(-1.0, (-(roomWidth / 2) + 0.5), 0.265), Cube_Steel);


	// Profiled Wood
	new_plane(cVector3d(-2.0, -(roomWidth / 2), 0.75), myLeftLablePW);
	new_object_cMultiMesh(cVector3d(-2.0, (-(roomWidth / 2) + 0.5), -0.005), Property_3ds, "table.3ds", 1);
	new_object_cMesh(cVector3d(-2.0, (-(roomWidth / 2) + 0.5), 0.265), Cube_WoodProfiled);

	// RIGHT SIDE

	// Ice
	new_plane(cVector3d(2.0, (roomWidth / 2), 0.75), myRightLableFW);
	new_object_cMultiMesh(cVector3d(2.0, ((roomWidth / 2) - 0.5), -0.005), Property_3ds, "table.3ds", 1);
	new_object_cMesh(cVector3d(2.0, ((roomWidth / 2) - 0.5), 0.265), Cube_Ice);

	// Cold Granite
	new_plane(cVector3d(1.0, (roomWidth / 2), 0.75), myRightLableCG);
	new_object_cMultiMesh(cVector3d(1.0, ((roomWidth / 2) - 0.5), -0.005), Property_3ds, "table.3ds", 1);
	new_object_cMesh(cVector3d(1.0, ((roomWidth / 2) - 0.5), 0.265), Cube_Granite);

	// Lava
	new_plane(cVector3d(0.0, (roomWidth / 2), 0.75), myRightLableSL);
	new_object_cMultiMesh(cVector3d(0.0, ((roomWidth / 2) - 0.5), -0.005), Property_3ds, "table.3ds", 1);
	new_object_cMesh(cVector3d(0.0, ((roomWidth / 2) - 0.5), 0.265), Cube_Lava);

	// Glowing Metal
	new_plane(cVector3d(-1.0, (roomWidth / 2), 0.75), myRightLableGS);
	new_plane(cVector3d(-1.0, (roomWidth / 2), 0.5), myRightLableCaution);
	new_object_cMultiMesh(cVector3d(-1.0, ((roomWidth / 2) - 0.5), -0.005), Property_3ds, "table.3ds", 1);
	new_object_cMesh(cVector3d(-1.0, ((roomWidth / 2) - 0.5), 0.265), Cube_GlowingMetal);

	// Rubber
	new_plane(cVector3d(-2.0, (roomWidth / 2), 0.75), myRightLablePR);
	new_object_cMultiMesh(cVector3d(-2.0, ((roomWidth / 2) - 0.5), -0.005), Property_3ds, "table.3ds", 1);
	new_object_cMesh(cVector3d(-2.0, ((roomWidth / 2) - 0.5), 0.265), Cube_Rubber);

	// Print how many objects got created
	cout << "Created " << objectCounter + 1 << " objects." << endl;

	// Print how many 3ds objects got created
	cout << "Created " << object3dsCounter + 1 << " 3ds objects." << endl;

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

		// check if inside a temperature region
		checkTempRegions();

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

//==============================================================================
// END OF MAIN FUNCTION
//==============================================================================

//------------------------------------------------------------------------------
// IMPLEMENTATIONS OF FUNCTIONS
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
			if (event.key.keysym.sym == SDLK_3)
			{
				keyState[(unsigned char)'3'] = 1;
			}
			if (event.key.keysym.sym == SDLK_4)
			{
				keyState[(unsigned char)'4'] = 1;
			}
			if (event.key.keysym.sym == SDLK_5)
			{
				keyState[(unsigned char)'5'] = 1;
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
			if (event.key.keysym.sym == SDLK_y)
			{
				keyState[(unsigned char)'y'] = 1;
			}
			if (event.key.keysym.sym == SDLK_c)
			{
				keyState[(unsigned char)'c'] = 1;
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
			if (event.key.keysym.sym == SDLK_3)
			{
				keyState[(unsigned char)'3'] = 0;
			}
			if (event.key.keysym.sym == SDLK_4)
			{
				keyState[(unsigned char)'4'] = 0;
			}
			if (event.key.keysym.sym == SDLK_5)
			{
				keyState[(unsigned char)'5'] = 0;
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
			if (event.key.keysym.sym == SDLK_y)
			{
				keyState[(unsigned char)'y'] = 0;
			}
			if (event.key.keysym.sym == SDLK_c)
			{
				keyState[(unsigned char)'c'] = 0;
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
	/*
	if (keyState[(unsigned char)'y'] == 1) // Turn down/up
	{
		if (currentAngle < 40 && currentAngle > -40)
			currentAngleV += rotationalSpeed;
	}
	if (keyState[(unsigned char)'c'] == 1) // Turn down/up
	{
		if (currentAngle < 40 && currentAngle > -40)
			currentAngleV -= rotationalSpeed;
	}
	*/
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
		if (myTemp != heating)
		{
			sendHot('2');
			cout << "Enable Heating!" << endl;
			myTemp = heating;
		}
	}
	if (keyState[(unsigned char)'2'] == 1) // special function 2
	{
		if (myTemp != cooling)
		{
			sendCold('2');
			cout << "Enable Cooling!" << endl;
			myTemp = cooling;
		}
	}
	if (keyState[(unsigned char)'3'] == 1) // special function 3
	{
		if (myTemp != standby)
		{
			sendReset();
			cout << "Disable Peltier element!" << endl;
			myTemp = standby;
		}
	}
	/*
	if (keyState[(unsigned char)'4'] == 1) // special function 4
	{
		cVector3d devPosition = tool->getDeviceGlobalPos();
		cout << "tool->getDevGlobPos = (";
		printf("%.2f/", devPosition.x());
		printf("%.2f/", devPosition.y());
		printf("%.2f)\n", devPosition.z());
		
		cSleepMs(100);
	}
	*/

	// recalculate the viewing direction
	currentDirection = cVector3d(-cos(currentAngle), -sin(currentAngle), 0.0);

	// recalculate the direction of the "up" vector
	camera->set(currentPosition, currentPosition + currentDirection, cVector3d(0, 0, 1));   
}

//------------------------------------------------------------------------------

void close(void)
{
	// Disable heating and cooling
	sendReset();

	// stop the simulation
	simulationRunning = false;

	// wait for graphics and haptics loops to terminate
	while (!simulationFinished) { cSleepMs(100); }

	/*
	// deleting the audio device object
	if (audioDevice != 0)
	{
		cout << "Deleting audioDevice" << endl;
		delete audioDevice;
	}

	for (int i = 0; i < audioBufferCounter; i++)
	{
		cout << "Deleting audioBuffer[" << i << "]" << endl;
		//delete audioBuffer[i];
	}

	for (int i = 0; i < objectCounter; i++)
		delete object[i];
	
	for (int i = 0; i < tempRegionCounter; i++)
	{
		delete tempRegion[i];
	}
	*/

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
		//cMatrix3d RotForce = cMatrix3d(cos(currentAngle), sin(currentAngle), 0.0, -sin(currentAngle), cos(currentAngle), cos(currentAngleV), -sin(currentAngleV), sin(currentAngleV), cos(currentAngleV));
		cMatrix3d Rot = cMatrix3d(cos(currentAngle), -sin(currentAngle), 0.0, sin(currentAngle), cos(currentAngle), 0.0, 0.0, 0.0, 1.0);
		//cMatrix3d Rot = cMatrix3d(cos(currentAngle), -sin(currentAngle), 0.0, sin(currentAngle), cos(currentAngle), cos(currentAngleV), -sin(currentAngleV), sin(currentAngleV), cos(currentAngleV));
		tool->setDeviceGlobalRot(Rot);
		cVector3d tmp = tool->getDeviceLocalPos();
		tool->setDeviceGlobalPos(Rot*tmp + currentPosition + currentDirection);

		// compute interaction forces
		tool->computeInteractionForces();

		tool->setDeviceGlobalTorque(RotForce*(tool->getDeviceGlobalTorque()));
		tool->setDeviceGlobalForce(RotForce*(tool->getDeviceGlobalForce()));

		// send forces to haptic device
		tool->applyToDevice();
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

void drawCoordinates(cVector3d position, double length, double width)
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

int new_object_cMesh(cVector3d position, MyProperties properties)
{
	if (objectCounter < MAX_OBJECT_COUNT)
	{
		//cout << "Creating new object Nr. " << objectCounter+1 << endl;
	}
	else
	{
		cout << "Error: Could not create new object. Maximal number of objects reached!" << endl;
		return -1;
	}

	// create a virtual mesh
	object[objectCounter] = new cMesh();

	// add object to world
	world->addChild(object[objectCounter]);

	// set the position of the object at the center of the world
	object[objectCounter]->setLocalPos(position);

	switch (properties.shape)
	{
	case(cube) :
		// create cube
		chai3d::cCreateBox(object[objectCounter], properties.size.x(), properties.size.y(), properties.size.z());
		break;
	case (sphere) :
		// create sphere
		chai3d::cCreateSphere(object[objectCounter], (const double)properties.size.length() / 2.5);
		break;
	case(cylinder) :
		chai3d::cCreateCylinder(object[objectCounter], (const double)properties.size.z(), cVector3d(properties.size.x(), properties.size.y(), 0.0).length() / 2);
		break;

	case(complex3ds) :
		
		break;
	}

	// create a texture
	cTexture2dPtr texture = cTexture2d::create();

	// load texture image from file
	if (texture->loadFromFile(RESOURCE_PATH(STR_ADD("images/", properties.textureImage))) != 1)
	{
		cout << "ERROR: Cannot load texture file!" << endl;
	}

	// apply texture to object
	object[objectCounter]->setTexture(texture);

	// enable texture rendering 
	object[objectCounter]->setUseTexture(true);

	// Since we don't need to see our polygons from both sides, we enable culling.
	object[objectCounter]->setUseCulling(true);

	// set material properties to light gray
	object[objectCounter]->m_material->setWhite();

	// compute collision detection algorithm
	object[objectCounter]->createAABBCollisionDetector(TOOL_RADIUS);

	// define a default stiffness for the object
	object[objectCounter]->m_material->setStiffness(properties.stiffness * maxStiffness);

	// define some static friction
	object[objectCounter]->m_material->setStaticFriction(properties.staticFriction);

	// define some dynamic friction
	object[objectCounter]->m_material->setDynamicFriction(properties.dynamicFriction);

	// define some texture rendering
	object[objectCounter]->m_material->setTextureLevel(properties.textureLevel);

	// render triangles haptically on front side only
	object[objectCounter]->m_material->setHapticTriangleSides(true, false);

	// create a normal texture
	cNormalMapPtr normalMap = cNormalMap::create();

	// load normal map from file
	if (normalMap->loadFromFile(RESOURCE_PATH(STR_ADD("images/", properties.normalImage))) != 1)
	{
		//cout << "ERROR: Cannot load normal map file!" << endl;
		normalMap->createMap(object[objectCounter]->m_texture);
	}

	// assign normal map to object
	object[objectCounter]->m_normalMap = normalMap;

	// compute surface normals
	object[objectCounter]->computeAllNormals();


	// #################################################################
	// THIS RISES PROBLEMS FOR SHPERES !!!

	// compute tangent vectors

	if (properties.shape != sphere)
		object[objectCounter]->m_triangles->computeBTN();

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
	object[objectCounter]->setShaderProgram(programShader);

	// link program shader
	programShader->linkProgram();

	// set uniforms
	programShader->setUniformi("uColorMap", 0);
	programShader->setUniformi("uShadowMap", 0);
	programShader->setUniformi("uNormalMap", 2);
	programShader->setUniformf("uInvRadius", 0.0f);

	// set the orientation
	object[objectCounter]->rotateAboutLocalAxisDeg(properties.orientation.axis, properties.orientation.rotation);

	//--------------------------------------------------------------------------
	// SETUP AUDIO MATERIAL
	//--------------------------------------------------------------------------
#if 1
	// check if audio gain is bigger than zero
	if (properties.audioGain > 0.0f)
	{
		if (audioBufferCounter < MAX_AUDIOBUFFER_COUNT)
		{
			// create an audio buffer and load audio wave file
			audioBuffer[audioBufferCounter] = audioDevice->newAudioBuffer();

			// load audio from file
			if (audioBuffer[audioBufferCounter]->loadFromFile(RESOURCE_PATH((STR_ADD("sounds/", properties.audio)))) != 1)
			{
				cout << "ERROR: Cannot load audio file!" << endl;
			}

			// here we convert all files to mono. this allows for 3D sound support. if this code
			// is commented files are kept in stereo format and 3D sound is disabled. Compare both!
			audioBuffer[audioBufferCounter]->convertToMono();

			// set audio properties
			object[objectCounter]->m_material->setAudioImpactBuffer(audioBuffer[audioBufferCounter]);
			object[objectCounter]->m_material->setAudioFrictionBuffer(audioBuffer[audioBufferCounter]);
			object[objectCounter]->m_material->setAudioFrictionGain((const double)properties.audioGain);
			object[objectCounter]->m_material->setAudioFrictionPitchGain((const double)properties.audioPitchGain);
			object[objectCounter]->m_material->setAudioFrictionPitchOffset((const double)properties.audioPitchOffset);

			// increment counter
			audioBufferCounter++;
		}
		else
		{
			cout << "ERROR: Cannot create an audio buffer for this object. Maximal audio buffer count reached!" << endl;
		}
	}
#endif

	//--------------------------------------------------------------------------
	// SETUP TEMPERATURE REGIONS
	//--------------------------------------------------------------------------
#if 1	
	// if temperature is not normal (cold or hot) assign heat region
	if (properties.temperature != 3)
	{
		if (tempRegionCounter < MAX_REGIONS_COUNT)
		{
			// create a region where a temperature needs to be checked
			tempRegion[tempRegionCounter] = new MyRegions(cVector3d(position), properties.size, properties.temperature);

			//cout << "  -> New temp region created (Nr." << tempRegionCounter+1 << ")" << endl;

			//cout << "size (x/y/z): (" << properties.size.x() << "/" << properties.size.y() << "/" << properties.size.z() << ")" << endl;

			// increment counter
			tempRegionCounter++;
		}
		else
		{
			cout << "ERROR: No region for detecting temperature can be assigned to this object!" << endl;
		}
	}
#endif
	// incrementing counter
	objectCounter++;

	return 0;
}

//------------------------------------------------------------------------------

int new_object_cMultiMesh(cVector3d position, MyProperties properties, string objectFile, double scalingFactor)
{
	if (object3dsCounter < MAX_OBJECT_COUNT)
	{
		//cout << "Creating new object Nr. " << object3dsCounter + 1 << " -> Used 3ds file: " << objectFile << endl;
	}
	else
	{
		cout << "Error: Could not create new object. Maximal number of objects reached!" << endl;
		return -1;
	}

	// create a virtual mesh
	object3ds[object3dsCounter] = new cMultiMesh();

	// add object to world
	world->addChild(object3ds[object3dsCounter]);

	// set the position of the object at the center of the world
	object3ds[object3dsCounter]->setLocalPos(position);

	// load normal map from file
	if (object3ds[object3dsCounter]->loadFromFile(RESOURCE_PATH(STR_ADD("3ds/", objectFile))) != 1)
	{
		cout << "ERROR: Cannot load 3ds file!" << endl;
	}

	// get dimensions of object
	object3ds[object3dsCounter]->computeBoundaryBox(true);
	double size = cSub(object3ds[object3dsCounter]->getBoundaryMax(), object3ds[object3dsCounter]->getBoundaryMin()).length();

	// resize object to screen
	if (size > 0.001)
	{
		object3ds[object3dsCounter]->scale(1 / (size * scalingFactor));
	}

	// create a texture
	cTexture2dPtr texture = cTexture2d::create();

	// load texture image from file
	if (texture->loadFromFile(RESOURCE_PATH(STR_ADD("images/", properties.textureImage))) != 1)
	{
		cout << "ERROR: Cannot load texture file!" << endl;
	}

	// apply texture to object
	object3ds[object3dsCounter]->setTexture(texture);

	// enable texture rendering 
	object3ds[object3dsCounter]->setUseTexture(true);

	// set material properties to light gray
	object3ds[object3dsCounter]->m_material->setBrown();

	// compute collision detection algorithm
	object3ds[object3dsCounter]->createAABBCollisionDetector(.02);

	// disable culling so that faces are rendered on both sides
	object3ds[object3dsCounter]->setUseCulling(false);

	// enable display list for faster graphic rendering
	object3ds[object3dsCounter]->setUseDisplayList(true);

	// center object in scene
	//object3ds[object3dsCounter]->setLocalPos(-1.0 * object3ds[object3dsCounter]->getBoundaryCenter());

	// rotate object in scene
	object3ds[object3dsCounter]->rotateExtrinsicEulerAnglesDeg(0, 0, 0, C_EULER_ORDER_XYZ);

#if 1

	// set haptic properties
	cMaterial mat;
	mat.setHapticTriangleSides(true, true);
	mat.setStiffness(properties.stiffness * maxStiffness);
	mat.setStaticFriction(properties.staticFriction);
	mat.setDynamicFriction(properties.dynamicFriction);
	object3ds[object3dsCounter]->setMaterial(mat);
	object3ds[object3dsCounter]->computeAllNormals();

#else

	// Since we don't need to see our polygons from both sides, we enable culling.
	object3ds[object3dsCounter]->setUseCulling(true);

	// set material properties to light gray
	object3ds[object3dsCounter]->m_material->setWhite();

	// compute collision detection algorithm
	object3ds[object3dsCounter]->createAABBCollisionDetector(TOOL_RADIUS);

	// define a default stiffness for the object
	object3ds[object3dsCounter]->m_material->setStiffness(properties.stiffness * maxStiffness);

	// define some static friction
	object3ds[object3dsCounter]->m_material->setStaticFriction(properties.staticFriction);

	// define some dynamic friction
	object3ds[object3dsCounter]->m_material->setDynamicFriction(properties.dynamicFriction);

	// define some texture rendering
	object3ds[object3dsCounter]->m_material->setTextureLevel(properties.textureLevel);

	// render triangles haptically on front side only
	object3ds[object3dsCounter]->m_material->setHapticTriangleSides(true, false);

	// create a normal texture
	cNormalMapPtr normalMap = cNormalMap::create();

	// load normal map from file
	if (normalMap->loadFromFile(RESOURCE_PATH(STR_ADD("images/", properties.normalImage))) != 1)
	{
		cout << "ERROR: Cannot load normal map file!" << endl;
		normalMap->createMap(object3ds[object3dsCounter]->m_texture);
	}

	// assign normal map to object
	object3ds[object3dsCounter]->m_normalMap = normalMap;

	// compute surface normals
	object3ds[object3dsCounter]->computeAllNormals();

#endif

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
	object3ds[object3dsCounter]->setShaderProgram(programShader);

	// link program shader
	programShader->linkProgram();

	// set uniforms
	programShader->setUniformi("uColorMap", 0);
	programShader->setUniformi("uShadowMap", 0);
	programShader->setUniformi("uNormalMap", 2);
	programShader->setUniformf("uInvRadius", 0.0f);

	// set the orientation
	object3ds[object3dsCounter]->rotateAboutLocalAxisDeg(properties.orientation.axis, properties.orientation.rotation);

	// incrementing counter
	object3dsCounter++;

	return 0;
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

//------------------------------------------------------------------------------

int checkTempRegions()
{
	int result = 0;

	// update position and orientation of tool
	//tool->updateFromDevice();

	// read tools position
	cVector3d devPosition = tool->getDeviceGlobalPos();
	
	cVector3d distance = cVector3d(0, 0, 0);

	// check each temperature region
	for (int i = 0; i < tempRegionCounter; i++)
	{
		// calculate distance between center of temperature region and 
		distance = devPosition - tempRegion[i]->position;

		// check if inside region
		if (distance.length() < 0.5)
		{
			//cout << "Inside region [" << i << "] Distance = ";
			//printf("%.3f\n", distance.length());

			// check the if haptics device is in contact with any surface
			double force = cVector3d(tool->getDeviceGlobalForce()).length();

			if (force > 0.01)
			{
				// send the respective string + the intensity
				sendTemperature(tempRegion[i]->temperature);
				cout << "IN CONTACT!    \r";
			}
			else
			{
				// decide which string to send back before the intensity (to make sure only the enable-relais is used)
				if (tempRegion[i]->temperature < 3)
					sendCold('0');
				else
					sendHot('0');

				cout << "NOT IN CONTACT!\r";
			}

			result = 1;
		}
		else
		{
			cout << "                   \r";
		
			// set return value to zero
			result = 0;
		}
	}
	return result;
}

