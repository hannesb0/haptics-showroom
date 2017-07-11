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
// CHAI3D
//------------------------------------------------------------------------------

// a world that contains all objects of the virtual environment
cWorld* world;

// a camera to render the world in the window display
cCamera* camera;

// a light source
cSpotLight *light;
//cDirectionalLight *light;

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

// indicates if the haptic simulation currently running
bool simulationRunning = false;

// indicates if the haptic simulation has terminated
bool simulationFinished = false;

// frequency counter to measure the simulation haptic rate
cFrequencyCounter frequencyCounter;

// NEW ########################

// new path to the resources (included in the repository)
string resourcesPath;

// Initial position : on +Z //change for testing to: cVector3d(1.0, 0.0, 0.2) For Presentation:cVector3d(2.0, 0.0, 0.2)
//cVector3d currentPosition = cVector3d(1.3, 0.0, 0.2);
cVector3d currentPosition = cVector3d(0.6, 0.3, 0.0);
cVector3d currentDirection = cVector3d(1.0, 0.0, 0.0);

// 
double currentAngle = 0;
double speed = 0.005;
double rotationalSpeed = 0.005;

// variable to store state of keys
unsigned int keyState[255];



//------------------------------------------------------------------------------
// OCULUS RIFT
//------------------------------------------------------------------------------

// display context
cOVRRenderContext renderContext;

// oculus device
cOVRDevice oculusVR;

//------------------------------------------------------------------------------
// DECLARED MACROS
//------------------------------------------------------------------------------

// convert to resource path
#define RESOURCE_PATH(p)    (char*)((resourceRoot+string(p)).c_str())
#define PROJECT_FOLDER		haptics-showroom-V02
#define WINDOW_SIZE_W		1000
#define WINDOW_SIZE_H		1000
#define TOOL_RADIUS			0.02
#define TOOL_WORKSPACE		0.3

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
	string resourceRoot = string(argv[0]).substr(0, string(argv[0]).find_last_of("/\\") + 1);
	resourcesPath = resourceRoot + string("../examples/SDL/PROJECT_FOLDER/resources/");


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
	light = new cSpotLight(world);
	//light = new cDirectionalLight(world);

	// add light to world
	world->addChild(light);

	// enable light source
	light->setEnabled(true);

	// position the light source
	light->setLocalPos(3.5, 2.0, 0.0);

	// define the direction of the light beam
	light->setDir(-3.5, -2.0, 0.0);

	// set light cone half angle
	light->setCutOffAngleDeg(50);
	//light->m_ambient.set(0.4f, 0.4f, 0.4f);
	//light->m_diffuse.set(0.8f, 0.8f, 0.8f);
	//light->m_specular.set(1.0f, 1.0f, 1.0f);


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
	//double toolRadius = TOOL_RADIUS;

	// define a radius for the tool
	tool->setRadius(TOOL_RADIUS);
	tool2->setRadius(TOOL_RADIUS);

	// enable if objects in the scene are going to rotate of translate
	// or possibly collide against the tool. If the environment
	// is entirely static, you can set this parameter to "false"
	tool->enableDynamicObjects(true);
	tool2->enableDynamicObjects(true);

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
	object->setLocalPos(0.0, 0.0, 0.0);

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
	bool fileload2 = textureWall->loadFromFile("./resources/images/white-wall.png");
	if (!fileload2)
	{
#if defined(_MSVC)
		fileload2 = textureWall->loadFromFile("./resources/images/white-wall.png");
#endif
	}
	if (!(fileload && fileload2))
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


	if (keyState[(unsigned char)'a'] == 1) // Turn left //Add "Angular Jump"?
	{
		currentAngle += rotationalSpeed;
	}

	if (keyState[(unsigned char)'d'] == 1) // Turn right //Add "Angular Jump"?
	{
		currentAngle -= rotationalSpeed;
	}

	if (keyState[(unsigned char)' '] == 1) // Recenter
	{
		currentPosition = cVector3d(2.0, 0.0, 0.2);
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


	/*
	if (keyState[(unsigned char)'c'] == 1 ) // show nodes of GEL structure
	{
	sendSurfacePropertiesToController(true, 1, 5, 3, 3);
	BumpServo(1);
	}
	*/
	/*
	if (keyState[(unsigned char)'C'] == 1) // show nodes of GEL structure
	{

	BumpServo(4);
	}
	*/
	/*
	if (keyState[(unsigned char)'n'] == 0) // show nodes of GEL structure
	{
	PreCondN = true;
	}*/

	/*if ( currentAngle<-90)
	{
	currentAngle = -90;
	}
	if (currentAngle<-90)
	{
	currentAngle = 90;
	}*/


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


	currentDirection = cVector3d(-cos(currentAngle), -sin(currentAngle), 0.0);

	camera->set(currentPosition, currentPosition + currentDirection, cVector3d(0, 0, 1));   // direction of the "up" vector

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
		//tool->setDeviceGlobalRot(Rot);
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

	// exit haptics thread
	simulationFinished = true;
}

//------------------------------------------------------------------------------
