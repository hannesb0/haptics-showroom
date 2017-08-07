//==============================================================================
/*
    \author    
    \version   2.1.0
*/
//==============================================================================

//------------------------------------------------------------------------------
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//------------------------------------------------------------------------------
// CHAI3D
//----------------------------------------------------------------------------
#include "chai3d.h"
#include "GL/glut.h"

//------------------------------------------------------------------------------
#include "SDL.h"
#include "OVRRenderContext.h"
#include "OVRDevice.h"
//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;
//------------------------------------------------------------------------------

//----Surface Definition--------------------------------------------------------
#include "TexturedSurface.h"
//#include "FileManager.h"
#include "Library_Serial2Arduino.h"
#include "GlobalConstants.h"
#include "TexturedSurfaceDatabase.h"













// use OCULUS
bool useOculus = false;

// a world that contains all objects of the virtual environment
cWorld* world;

// a camera to render the world in the window display
cCamera* camera;

// a light source
cDirectionalLight *light;

// a haptic device handler
cHapticDeviceHandler* handler;

// a pointer to the current haptic device
cGenericHapticDevicePtr hapticDevice;
cGenericHapticDevicePtr hapticDevice2;

// force scale factor
double deviceForceScale;

// a virtual tool representing the haptic device in the scene
cToolCursor* tool;
cToolCursor* tool2;

double radius;




// Declare functions
// callback to render graphic scene
void updateGraphics(void);

// callback of GLUT timer
void graphicsTimer(int data);

// function that closes the application
void close(void);



// interaction stiffness between tool and deformable model 
double stiffness = 50;

// Build deformable model of huge cell
void BuildDynamicModel();


//--------------------------------------------------------------

// a virtual object
//vector<cMultiMesh*> column(NUMCOLUMN);
cMultiMesh *column;
// virtual textured objects
vector<cMesh*> mObjects(NUMSURFACES);
vector<TexturedSurface*> mSurfaceMaterials(NUMSURFACES);
vector<cAudioBuffer*> mAudioFrictionBuffer(NUMSURFACES);
vector<cTexture2dPtr> mTextures(NUMSURFACES);
vector<cNormalMapPtr> mNormalMaps(NUMSURFACES);

// indicates if the haptic simulation currently running
bool simulationRunning = false;

// indicates if the haptic simulation has terminated
bool simulationFinished = false;

// frequency counter to measure the simulation haptic rate
cFrequencyCounter frequencyCounter;

// audio device to play sound
cAudioDevice* audioDevice;





const double AUDIO_STARTUP_TIME = 10.0;
const double AUDIO_FREQUENCY = 300.0;
const double FORCE_MAG = 0.2;
const double AUDIO_GAIN = 2.0;

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
string path;

//------------------------------------------------------------------------------
// DECLARE STRUCTS
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// DECLARED FUNCTIONS
//------------------------------------------------------------------------------

// function that closes the application
void close(void);

// main haptics simulation loop
void updateHaptics(void);

// process keyboard events
void processEvents();

// function that computes movements through kye input
void computeMatricesFromInput();


#define BUTTON_UP   0
#define BUTTON_DOWN 1

#define MOUSE_LEFT   0
#define MOUSE_MIDDLE 1
#define MOUSE_RIGHT  2

unsigned int keyState[255];
unsigned char mouseState[3];
double deltaTime = 0.1;
int lastMouseX;
int lastMouseY;
// Initial position : on +Z
cVector3d currentPosition = cVector3d(1.3, 0.0, 0.2); //change for testing to: cVector3d(1.0, 0.0, 0.2) For Presentation:cVector3d(2.0, 0.0, 0.2)
cVector3d currentDirection = cVector3d(1.0, 0.0, 0.0);


// Initial Field of View
double initialFoV = 45.0;

double currentAngle = 0;
double speed = 0.01;
double rotationalSpeed = 0.01;


int main(int argc, char **argv)
{
	//--------------------------------------------------------------------------
	// INITIALIZATION
	//--------------------------------------------------------------------------

	cout << endl;
	cout << "-----------------------------------" << endl;
	cout << "CHAI3D" << endl;
	cout << "Finalproject" << endl;
	cout << "-----------------------------------" << endl << endl << endl;
	cout << "Keyboard Options:" << endl << endl;
	cout << "[ ] - Recenter view point" << endl;
	cout << "[q] - Exit application" << endl;
	cout << "[a] - Turn left" << endl;
	cout << "[d] - Turn right" << endl;
	cout << "[w] - Move forward" << endl;
	cout << "[s] - Move backwards" << endl;
	cout << endl << endl;

	// parse first arg to try and locate resources
	string resourceRoot = string(argv[0]).substr(0, string(argv[0]).find_last_of("/\\") + 1);
	path = resourceRoot + string("../examples/SDL/finalproject/resources/");

	//--------------------------------------------------------------------------
	// SETUP AUDIO FUNCTIONS
	//--------------------------------------------------------------------------

	InitSerial2Arduino();
	sendSurfacePropertiesToController(true, 1, 5, 3, 3);
	BumpServo(10);

	//--------------------------------------------------------------------------
	// SETUP DISPLAY CONTEXT
	//--------------------------------------------------------------------------

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
	{
		cout << "failed initialization" << endl;
		cSleepMs(1000);
		return 1;
	}

	if (!oculusVR.initVR() && useOculus)
	{
		cout << "failed to initialize Oculus" << endl;
		cSleepMs(1000);
		SDL_Quit();
		return 1;
	}

	if (useOculus)
	{
		ovrSizei hmdResolution = oculusVR.getResolution();
		ovrSizei windowSize = { hmdResolution.w / 2, hmdResolution.h / 2 };
		renderContext.init("CHAI3D", 100, 100, windowSize.w, windowSize.h);
	}
	else
		renderContext.init("CHAI3D", 100, 100, 1000, 1000);


	SDL_ShowCursor(SDL_DISABLE);

	// setup GLUT options
	//glutDisplayFunc(updateGraphics);

	if (glewInit() != GLEW_OK)
	{
		if (useOculus)
		{
			oculusVR.destroyVR();
			renderContext.destroy();
		}

		SDL_Quit();
		return 1;
	}
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
	camera->setClippingPlanes(0.1, 50.0);

	// create a light source
	light = new cDirectionalLight(world);

	// add light to world
	world->addChild(light);

	// enable light source
	light->setEnabled(true);

	// position the light source
	light->setLocalPos(0, 0, 30.0);

	// define the direction of the light beam
	light->setDir(0, 0, -30.0);

	// set lighting conditions
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

	// connect the haptic device to the tool
	tool->setHapticDevice(hapticDevice);

	// define the radius of the tool (sphere)
	double toolRadius = 0.02;

	// define a radius for the tool
	tool->setRadius(toolRadius);

	// define a scale factor between the force perceived at the cursor and the
	// forces actually sent to the haptic device
	deviceForceScale = 5.0;

	// enable if objects in the scene are going to rotate of translate
	// or possibly collide against the tool. If the environment
	// is entirely static, you can set this parameter to "false"
	tool->enableDynamicObjects(true);

	// map the physical workspace of the haptic device to a larger virtual workspace.
	tool->setWorkspaceRadius(0.3);

	// haptic forces are enabled only if small forces are first sent to the device;
	// this mode avoids the force spike that occurs when the application starts when 
	// the tool is located inside an object for instance. 
	tool->setWaitForSmallForce(true);

	// start the haptic tool
	tool->start();





	// create a 3D tool and add it to the world
	tool2 = new cToolCursor(world);
	world->addChild(tool2);

	// connect the haptic device to the tool
	tool2->setHapticDevice(hapticDevice2);

	// define the radius of the tool2 (sphere)
	toolRadius = 0.02;

	// define a radius for the tool
	tool2->setRadius(toolRadius);

	// define a scale factor between the force perceived at the cursor and the
	// forces actually sent to the haptic device
	deviceForceScale = 5.0;

	// enable if objects in the scene are going to rotate of translate
	// or possibly collide against the tool. If the environment
	// is entirely static, you can set this parameter to "false"
	tool2->enableDynamicObjects(true);

	// map the physical workspace of the haptic device to a larger virtual workspace.
	tool2->setWorkspaceRadius(0.3);

	// haptic forces are enabled only if small forces are first sent to the device;
	// this mode avoids the force spike that occurs when the application starts when 
	// the tool is located inside an object for instance. 
	tool2->setWaitForSmallForce(true);

	// start the haptic tool
	tool2->start();

	//--------------------------------------------------------------------------
	// SETUP AUDIO MATERIAL
	//--------------------------------------------------------------------------

	// create an audio device to play sounds
	audioDevice = new cAudioDevice();

	// create an audio source for this tool.
	tool->createAudioSource(audioDevice);


	//--------------------------------------------------------------------------
	// CREATE OBJECT
	//--------------------------------------------------------------------------

	// read the scale factor between the physical workspace of the haptic
	// device and the virtual workspace defined for the tool
	double workspaceScaleFactor = tool->getWorkspaceScaleFactor();

	// stiffness properties
	double maxStiffness = hapticDeviceInfo.m_maxLinearStiffness / workspaceScaleFactor;


    //--------------------------------------------------------------------------
    // CREATE SHADERS
    //--------------------------------------------------------------------------
	
    // create vertex shader
    cShaderPtr vertexShader = cShader::create(C_VERTEX_SHADER);

    // load vertex shader from file
    bool fileload = vertexShader->loadSourceFile("./resources/shaders/bump.vert");
    if (!fileload)
    {
#if defined(_MSVC)
        fileload = vertexShader->loadSourceFile("./resources/shaders/bump.vert");
#endif
    }

    // create fragment shader
    cShaderPtr fragmentShader = cShader::create(C_FRAGMENT_SHADER);

    // load fragment shader from file
    fileload = fragmentShader->loadSourceFile("./resources/shaders/bump.frag");
    if (!fileload)
    {
#if defined(_MSVC)
        fileload = fragmentShader->loadSourceFile("./resources/shaders/bump.frag");
#endif
    }

    // create program shader
    cShaderProgramPtr programShader = cShaderProgram::create();

    // assign vertex shader to program shader
    programShader->attachShader(vertexShader);

    // assign fragment shader to program shader
    programShader->attachShader(fragmentShader);

    // link program shader
    programShader->linkProgram();

    // set uniforms
    programShader->setUniformi("uColorMap", 0);
    programShader->setUniformi("uShadowMap", 0);
    programShader->setUniformi("uNormalMap", 2);
    programShader->setUniformf("uInvRadius", 0.0f);


	//--------------------------------------------------------------------------
	// CREATE TEXTURED SURFACES
	//--------------------------------------------------------------------------

	// LOAD TEXTURES
	for (int i = 0; i < NUMSURFACES; i++)
	{
		stringstream ss;
		ss << i;
		string textureStr = ss.str();
		string tmp = string("color") + textureStr + string(".png");

		mTextures[i] = cTexture2d::create();
		bool fileload = mTextures[i]->loadFromFile(path + "images/" + tmp);
		if (!fileload)
		{
#if defined(_MSVC)
			fileload = mTextures[i]->loadFromFile("resources/images/"+tmp);
#endif
		}
		if (!fileload)
		{
			cout << "Error - Texture images failed to load correctly." << endl;
			close();
			return (-1);
		}
	}

	// LOAD NORMAL MAPS
	for (int i = 0; i < NUMSURFACES; i++)
	{
		stringstream ss;
		ss << i;
		string textureStr = ss.str();
		string tmp = string("normal") + textureStr + string(".png");

		mNormalMaps[i] = cNormalMap::create();
		bool fileload = mNormalMaps[i]->loadFromFile(path + "images/" + tmp);
		if (!fileload)
		{
#if defined(_MSVC)
			fileload = mNormalMaps[i]->loadFromFile("resources/images/"+tmp);
#endif
		}
		if (!fileload)
		{
			cout << "Error - Nomral images failed to load correctly." << endl;
			close();
			return (-1);
		}
	}

	// LOAD AUDIO SIGNALS
	for (int i = 0; i < NUMSURFACES; i++)
	{
		stringstream ss;
		ss << i;
		string textureStr = ss.str();
		string tmp = string("audio") + textureStr + string(".wav");

		mAudioFrictionBuffer[i] = audioDevice->newAudioBuffer();
		bool fileload = mAudioFrictionBuffer[i]->loadFromFile(path + "sounds/" + tmp);
		if (!fileload)
		{
#if defined(_MSVC)
			fileload = mAudioFrictionBuffer[i]->loadFromFile("resources/sounds/"+ tmp);
#endif
		}
		if (!fileload)
		{
			cout << "Error - Audio signal failed to load correctly." << endl;
			close();
			return (-1);
		}

		// convert all files to mono. this allows for 3D sound support.
		mAudioFrictionBuffer[i]->convertToMono();
	}

	// CREATE TEXTURED SURFACES
	for (int i = 0; i < NUMSURFACES/2; i++)
	{
		//std::cout << myTexturedSurfaceDatabase[i][4] << endl;

		mObjects[i] = new cMesh();
		mSurfaceMaterials[i] = new TexturedSurface(mObjects[i], cVector3d(0, (i*0.3-1.5), -0.01), dSize, mTextures[i],
													mNormalMaps[i], myTexturedSurfaceDatabase[i][0], myTexturedSurfaceDatabase[i][1],
													myTexturedSurfaceDatabase[i][2], myTexturedSurfaceDatabase[i][3], myTexturedSurfaceDatabase[i][4],
													myTexturedSurfaceDatabase[i][5], programShader, mAudioFrictionBuffer[i], myTexturedSurfaceDatabase[i][6], myTexturedSurfaceDatabase[i][7]);
		mSurfaceMaterials[i]->CreateTexturedSurface(mSurfaceMaterials[i], world, toolRadius, maxStiffness);
	}

	for (int i = 5; i <7 ; i++)
	{
		//std::cout << myTexturedSurfaceDatabase[i][4] << endl;

		mObjects[i] = new cMesh();
		mSurfaceMaterials[i] = new TexturedSurface(mObjects[i], cVector3d(0, (i*0.3 - 1.5), 0.025), dSize, mTextures[i],
			mNormalMaps[i], myTexturedSurfaceDatabase[i][0], myTexturedSurfaceDatabase[i][1],
			myTexturedSurfaceDatabase[i][2], myTexturedSurfaceDatabase[i][3], myTexturedSurfaceDatabase[i][4],
			myTexturedSurfaceDatabase[i][5], programShader, mAudioFrictionBuffer[i], myTexturedSurfaceDatabase[i][6], myTexturedSurfaceDatabase[i][7]);
		mSurfaceMaterials[i]->CreateTexturedSurface(mSurfaceMaterials[i], world, toolRadius, maxStiffness);
	}

	for (int i = 9; i < 10; i++)
	{
		//std::cout << myTexturedSurfaceDatabase[i][4] << endl;

		mObjects[i] = new cMesh();
		mSurfaceMaterials[i] = new TexturedSurface(mObjects[i], cVector3d(0, (7*0.3 - 1.5), 0.025), dSize, mTextures[i],
			mNormalMaps[i], myTexturedSurfaceDatabase[i][0], myTexturedSurfaceDatabase[i][1],
			myTexturedSurfaceDatabase[i][2], myTexturedSurfaceDatabase[i][3], myTexturedSurfaceDatabase[i][4],
			myTexturedSurfaceDatabase[i][5], programShader, mAudioFrictionBuffer[i], myTexturedSurfaceDatabase[i][6], myTexturedSurfaceDatabase[i][7]);
		mSurfaceMaterials[i]->CreateTexturedSurface(mSurfaceMaterials[i], world, toolRadius, maxStiffness);
	}


    //--------------------------------------------------------------------------
    // CREATE ENVIRONMENT ROOM
    //--------------------------------------------------------------------------

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

    //--------------------------------------------------------------------------
    // START SIMULATION
    //--------------------------------------------------------------------------

    // create a thread which starts the main haptics rendering loop
    cThread* hapticsThread = new cThread();
    hapticsThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);

    // setup callback when application exits
    //atexit(close);

	// start the main graphics rendering loop
	/*glutTimerFunc(50, graphicsTimer, 0);
	glutMainLoop();*/

    //--------------------------------------------------------------------------
    // MAIN GRAPHIC LOOP
    //--------------------------------------------------------------------------



    while (!simulationFinished)
    {
        // handle key presses
        processEvents();

		computeMatricesFromInput();

		//defWorld->updateSkins(true);

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
			if (useOculus)
			{
				oculusVR.onEyeRender(eyeIndex, projectionMatrix, modelViewMatrix);
			}
			camera->m_useCustomProjectionMatrix = useOculus;
			if (useOculus)
			{
				camera->m_projectionMatrix = projectionMatrix;
			}
			camera->m_useCustomModelViewMatrix = useOculus;
			if (useOculus)
			{
				camera->m_modelViewMatrix = modelViewMatrix;
			}


            // render world
			if (useOculus)
			{
				ovrSizei size = oculusVR.getEyeTextureSize(eyeIndex);

				camera->renderView(size.w, size.h, 0, C_STEREO_LEFT_EYE, false);
			}
			else
				camera->renderView(1000,1000, 0, C_STEREO_LEFT_EYE, false);

            // finalize rendering  

			if (useOculus)
			{
				oculusVR.onEyeRenderFinish(eyeIndex);
			}
        }

        // update frames
		if (useOculus)
		{
			oculusVR.submitFrame();
			oculusVR.blitMirror();
		}
        SDL_GL_SwapWindow(renderContext.window);
    }

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
            // esc
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                close();
                break;
            }

            // spacebar
            /*if (event.key.keysym.sym == SDLK_SPACE)
            {
                oculusVR.recenterPose();
                break;
            }*/


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
			if (event.key.keysym.sym == SDLK_SPACE)
			{
				keyState[(unsigned char)' '] = 1;
			}
			if (event.key.keysym.sym == SDLK_h)
			{
				keyState[(unsigned char)'h'] = 1;
			}
			if (event.key.keysym.sym == SDLK_n)
			{
				keyState[(unsigned char)'n'] = 1;
			}
            break;


		case SDL_KEYUP:
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
			if (event.key.keysym.sym == SDLK_SPACE)
			{
				keyState[(unsigned char)' '] = 0;
			}
			if (event.key.keysym.sym == SDLK_h)
			{
				keyState[(unsigned char)'h'] = 0;
			}
			if (event.key.keysym.sym == SDLK_n)
			{
				keyState[(unsigned char)'n'] = 0;
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
	
	if (keyState[(unsigned char)'c'] == 1 ) // show nodes of GEL structure
	{
		sendSurfacePropertiesToController(true, 1, 5, 3, 3);
		BumpServo(1);
	}
	if (keyState[(unsigned char)'C'] == 1) // show nodes of GEL structure
	{

		BumpServo(4);
	}
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

	currentDirection = cVector3d(-cos(currentAngle), -sin(currentAngle), 0.0);

	camera->set(currentPosition, currentPosition + currentDirection, cVector3d(0, 0, 1));   // direction of the "up" vector

}

//-------------------------------------------------------------------------------

void close(void)
{
    // stop the simulation
    simulationRunning = false;

    // wait for graphics and haptics loops to terminate
    while (!simulationFinished) { cSleepMs(100); }

    // close haptic device
	if (tool != NULL)
		tool->stop();
	if (tool2 != NULL)
		tool2->stop();

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
		// DYNAMICS COMPUATION
		/////////////////////////////////////////////////////////////////////
		
		//// compute reaction forces
		cVector3d force(0.0, 0.0, 0.0);


		cVector3d pos = tool->getDeviceLocalPos();
		cVector3d tmpPos = pos;
		double deviceRadius = 0.03;





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
		tool->setDeviceGlobalPos(Rot*tmp+currentPosition+currentDirection);
		// compute interaction forces
		tool->computeInteractionForces();
		tool->setDeviceGlobalTorque(RotForce*(tool->getDeviceGlobalTorque()));
		tool->setDeviceGlobalForce(RotForce*(tool->getDeviceGlobalForce()));
        tool->applyToDevice();




		tool2->updateFromDevice();

		RotForce = cMatrix3d(cos(currentAngle), sin(currentAngle), 0.0, -sin(currentAngle), cos(currentAngle), 0.0, 0.0, 0.0, 1.0);
		Rot = cMatrix3d(cos(currentAngle), -sin(currentAngle), 0.0, sin(currentAngle), cos(currentAngle), 0.0, 0.0, 0.0, 1.0);
		//tool->setDeviceGlobalRot(Rot);

		tmp = tool2->getDeviceLocalPos();
		tool2->setDeviceGlobalPos(Rot*tmp + currentPosition + currentDirection);

		// compute interaction forces
		tool2->computeInteractionForces();
		tool2->setDeviceGlobalTorque(RotForce*(tool2->getDeviceGlobalTorque()));
		tool2->setDeviceGlobalForce(RotForce*(tool2->getDeviceGlobalForce()));


		tool2->applyToDevice();

    }

    // exit haptics thread
    simulationFinished = true;
}

//------------------------------------------------------------------------------

