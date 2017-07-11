//==============================================================================
/*
    Software License Agreement (BSD License)
    Copyright (c) 2003-2016, CHAI3D.
    (www.chai3d.org)

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
    copyright notice, this list of conditions and the following
    disclaimer in the documentation and/or other materials provided
    with the distribution.

    * Neither the name of CHAI3D nor the names of its contributors may
    be used to endorse or promote products derived from this software
    without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE. 

    \author    <http://www.chai3d.org>
    \author    Francois Conti
    \version   3.1.0 $Rev: 1907 $
*/
//==============================================================================

//------------------------------------------------------------------------------
#include "chai3d.h"
#include "GEL3D.h"
//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;
//------------------------------------------------------------------------------
#ifndef MACOSX
#include "GL/glut.h"
#else
#include "GLUT/glut.h"
#endif
//------------------------------------------------------------------------------

#include "math.h"
#include "Windows.h"

//#include "SurfaceMaterial.h"
//#include "FileManager.h"
#include "MySerial.h"
#include "TGAReader.h"
#include "GlobalConstants.h"
#include "Surface.h"
#include "Mouse.h"


#include <algorithm>    // std::random_shuffle
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand


#include <fstream>

//------------------------------------------------------------------------------
// GENERAL SETTINGS
//------------------------------------------------------------------------------

// stereo Mode
/*
    C_STEREO_DISABLED:            Stereo is disabled 
    C_STEREO_ACTIVE:              Active stereo for OpenGL NVDIA QUADRO cards
    C_STEREO_PASSIVE_LEFT_RIGHT:  Passive stereo where L/R images are rendered next to each other
    C_STEREO_PASSIVE_TOP_BOTTOM:  Passive stereo where L/R images are rendered above each other
*/
cStereoMode stereoMode = C_STEREO_DISABLED;

// fullscreen mode
bool fullscreen = false;

// mirrored display
bool mirroredDisplay = false;


//---------------------------------------------------------------------------
// DECLARED VARIABLES
//---------------------------------------------------------------------------

// a world that contains all objects of the virtual environment
cWorld* world;

// a camera to render the world in the window display
cCamera* camera;
cVector3d currentPos;

// a light source to illuminate the objects in the world
cDirectionalLight *light;

//tga read
int* mWidth;
int* mHeight;
unsigned char* pixels;

int lastPixelValue[2] = { 0, 0 };

Mouse* mouse;

int wrongOrigin = 1;
int debug_flag = 2;

vector<Surface*> SurfaceVector;
int currentSurfaceNo = 0;

//vector<SurfaceMaterial> mSurfaceMaterials(NUMSURFACES);

// audio device to play sound
cAudioDevice* audioDevice;

// audio buffers to store sound files
cAudioBuffer* audioBuffer1;
cAudioBuffer* audioBuffer2;
cAudioBuffer* audioBuffer3;
cAudioBuffer* audioBuffer4;

// audio buffer to store sound of a drill
cAudioBuffer* audioBuffer;

// audio source of a drill
cAudioSource* audioSource;


//FileManager* mFileManager;

MySerial* mSerial;
MySerial* mSerialTeleOp;

// flag to indicate if the haptic simulation currently running
bool simulationRunning = false;

// flag to indicate if the haptic simulation has terminated
bool simulationFinished = true;

// information about computer screen and GLUT display window
int screenW;
int screenH;
int windowW;
int windowH;
int windowPosX;
int windowPosY;

// root resource path
string resourceRoot;


//---------------------------------------------------------------------------
// GEL
//---------------------------------------------------------------------------

// deformable world
cGELWorld* defWorld;
cToolCursor* tool;
// object mesh
//cGELMesh* defObject;
cMesh*		object0;
cMesh* ball;
cMesh* ball2;

bool ballCreated = false;


// haptic device model
cShapeSphere* device;
double deviceRadius;

// radius of the dynamic model sphere (GEM)
double radius;


int surfaceNo = 0;




/* Experiment variables*/

const int NUMSUBJECTS = 20;
int currentSubject = 0;
int currentlyDisabledTactileFeature = 0;
bool enableVisual = true;

int currentSubjectRating[NUMBER_OF_SURFACES][6];
std::vector<int> experimentPermutationVector;
bool useExperimentalPermutation = true;
// random generator function:
int myrandom(int i) { return std::rand() % i; }

// create a font
cFont* font = NEW_CFONTCALIBRI40();
// create a label
cLabel* label = new cLabel(font);

string toStrVal(int i)
{
	stringstream ss;
	ss << i;
	string str = ss.str();
	return str;
}

void writeSubjectDataToFile(int currentSubject, int currentSubjectRating[NUMBER_OF_SURFACES][6])
{
	ofstream myfile;
	stringstream ss;
	ss << currentSubject;
	string str = ss.str();
	string filename = string("./experiment/Subject") + str + string(".txt");
	cout << endl;
	myfile.open(filename);
	cout << "Subject " << currentSubject << " ratings are:" << endl;
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 6; j++)
		{
		
			cout << currentSubjectRating[i][j] << "  ";
			myfile << currentSubjectRating[i][j] << "  ";
		}
		myfile << endl;
		cout << endl;
	}
	cout << endl << endl;
	myfile.close();
}





//---------------------------------------------------------------------------
// DECLARED FUNCTIONS
//---------------------------------------------------------------------------

// callback when the window display is resized
void resizeWindow(int w, int h);

// callback when a key is pressed
void keySelect(unsigned char key, int x, int y);

//display text
void displayText(float x, float y, int r, int g, int b, string str);

// callback to render graphic scene
void updateGraphics(void);

// callback of GLUT timer
void graphicsTimer(int data);

//mouse motion
void handlePassiveMouseMotion(int x, int y);

//mouse velocity
double getMouseVel(int x, int y);

bool hasMouseMoved(int x, int y);

pos predictMousePos(vector<pos> lastPosVector);

//Serial Communication
void initSerialCommunication();

//print key info to console window
void printBasicOutput();

//window size, fullscreen etc.
void setGlutSettings(int argc, char* argv[]);

//create mouse
void createMouse();

//load and update surfaces
void loadSurfaces();
void updateSurface(int surfaceNo);
void sendSurfacePropertiesToController();

//3D scenegraph, world, camera, light
void create3DSceneGraph();

//audio material
int setupAudio();

// function that closes the application
void close(void);


//---------------------------------------------------------------------------
// DECLARED MACROS
//---------------------------------------------------------------------------

// convert to resource path
#define RESOURCE_PATH(p)    (char*)((resourceRoot+string(p)).c_str())



int main(int argc, char* argv[])
{
	cout << "Enter current subject: " << endl;
	cin >> currentSubject;
	if (currentSubject < 0 || currentSubject > NUMSUBJECTS - 1) return -1;
	

	std::srand(unsigned(std::time(0)));

	// set some values:
	for (int i = 0; i<10; ++i) experimentPermutationVector.push_back(i); // 1 2 3 4 5 6 7 8 9

	// using built-in random generator:
	std::random_shuffle(experimentPermutationVector.begin(), experimentPermutationVector.end());

	// using myrandom:
	std::random_shuffle(experimentPermutationVector.begin(), experimentPermutationVector.end(), myrandom);

	// print out content:

	//for (std::vector<int>::iterator it = experimentPermutationVector.begin(); it != experimentPermutationVector.end(); ++it)
	//	std::cout << ' ' << *it;


	
	// Init Serial Communication
	initSerialCommunication();


    //-----------------------------------------------------------------------
    // INITIALIZATION
    //-----------------------------------------------------------------------

	printBasicOutput();

    // parse first arg to try and locate resources
    resourceRoot = string(argv[0]).substr(0,string(argv[0]).find_last_of("/\\")+1);


    //-----------------------------------------------------------------------
    // OPEN GL - WINDOW DISPLAY
    //-----------------------------------------------------------------------

	//initialize window size, fullscreen etc.
	setGlutSettings(argc, argv);

	world = new cWorld();

	//create mouse
	createMouse();

	//load surfaces
	loadSurfaces();

	// create an audio device to play sounds
	audioDevice = new cAudioDevice();




    //-----------------------------------------------------------------------
    // 3D - SCENEGRAPH
    //-----------------------------------------------------------------------

	//world, camera, light
	create3DSceneGraph();
	updateSurface(currentSurfaceNo);

    //--------------------------------------------------------------------------
    // WIDGETS
    //--------------------------------------------------------------------------

    // create a font
    cFont *font = NEW_CFONTCALIBRI20();

    // create a background
    cBackground* background = new cBackground();
    camera->m_backLayer->addChild(background);

    // set background properties
    background->setCornerColors(cColorf(1.00f, 1.00f, 1.00f),
                                cColorf(0.95f, 0.95f, 0.95f),
                                cColorf(0.85f, 0.85f, 0.85f),
                                cColorf(0.80f, 0.80f, 0.80f));

	

	displayText(100, 1100, 1, 0, 0, string("Current rating: ") + string("0") + string("    0: similar   1: weakly worse   2: worse   3:  much worse   4:  weakly better   5: better   6: much better"));

	// create a mesh
	cMesh* objectX = new cMesh();
	cMesh* objectY = new cMesh();
	cMesh* objectZ = new cMesh();
	ball = new cMesh();
	ball2 = new cMesh();

	cColorf* colorX = new cColorf();
	colorX->setRed();
	cColorf* colorY = new cColorf();
	colorX->setGreen();
	cColorf* colorZ = new cColorf();
	colorX->setBlue();

	//coordinate system
	cCreateArrow(objectX, 1, 0.01, 0.2, 0.05, false, 32U, cVector3d(1, 0, 0), cVector3d(0,0,0),*colorX);
	cCreateArrow(objectY, 1, 0.01, 0.2, 0.05, false, 32U, cVector3d(0, 1, 0), cVector3d(0, 0, 0), *colorY);
	cCreateArrow(objectZ, 1, 0.01, 0.2, 0.05, false, 32U, cVector3d(0, 0, 1), cVector3d(0, 0, 0), *colorZ);

	world->addChild(objectX);
	world->addChild(objectY);
	world->addChild(objectZ);

	ballCreated = true;

    //-----------------------------------------------------------------------
    // START SIMULATION
    //-----------------------------------------------------------------------

    // setup callback when application exits
    atexit(close);

    // start the main graphics rendering loop
    glutTimerFunc(50, graphicsTimer, 0);
    glutMainLoop();

    // exit
    return (0);
}

//---------------------------------------------------------------------------

void resizeWindow(int w, int h)
{
    windowW = w;
    windowH = h;
}

//---------------------------------------------------------------------------

void keySelect(unsigned char key, int x, int y)
{
	// option ESC: exit
	if ((key == 27) || (key == 'x'))
	{
		char buffer[] = { "RESET#1\n" };

		mSerial->WriteData(buffer, 8);
		// exit application
		exit(0);
	}

	// option s: show/hide skeleton
	else if (key == 'e')
	{
		//defObject->m_showSkeletonModel = !defObject->m_showSkeletonModel;

		//cout << "Experiment." << endl;

		currentlyDisabledTactileFeature++;
		if (currentlyDisabledTactileFeature > 5)
			currentlyDisabledTactileFeature = 0;

		sendSurfacePropertiesToController();
		cout << "Currently disabled feature: " << currentlyDisabledTactileFeature << endl;



	}
	else if (key == 'v')
	{
		enableVisual = !enableVisual;
		//defObject->m_showSkeletonModel = !defObject->m_showSkeletonModel;
	}



	// option q: toggle fullscreen
	else if (key == 'f')
	{
		if (fullscreen)
		{
			windowPosX = glutGet(GLUT_INIT_WINDOW_X);
			windowPosY = glutGet(GLUT_INIT_WINDOW_Y);
			windowW = glutGet(GLUT_INIT_WINDOW_WIDTH);
			windowH = glutGet(GLUT_INIT_WINDOW_HEIGHT);
			glutPositionWindow(windowPosX, windowPosY);
			glutReshapeWindow(windowW, windowH);
			fullscreen = false;
		}
		else
		{
			glutFullScreen();
			windowW = glutGet(GLUT_WINDOW_WIDTH);
			windowH = glutGet(GLUT_WINDOW_HEIGHT);
			mouse->setWindowSize(windowW, windowH);
			fullscreen = true;
		}
	}

	//hide and show mouse cursor
	else if (key == 'h') {
		glutSetCursor(GLUT_CURSOR_NONE);
	}
	else if (key == 's')
	{
		cout << "Write test data for subject " << currentSubject << " to file..." << endl;
		writeSubjectDataToFile(currentSubject, currentSubjectRating);
		cout << "done. " << endl;

		//glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
	}

	else if (key == 'H') {
		mouse->showPredictorSphere(false);
		glutPostRedisplay();
	}
	else if (key == 'S')
	{
		mouse->showPredictorSphere(true);
		glutPostRedisplay();
	}

	else if (key == 'm') {
		if (currentSurfaceNo == 0) {
			int newFriction = SurfaceVector.at(0)->getFriction() - 1;
			SurfaceVector.at(0)->setFriction(newFriction);
			sendSurfacePropertiesToController();
		}
	}

	else if (key == 'M') {
		if (currentSurfaceNo == 0) {
			int newFriction = SurfaceVector.at(0)->getFriction() + 1;
			SurfaceVector.at(0)->setFriction(newFriction);
			sendSurfacePropertiesToController();
		}
	}

	else if (key == 'c') {
		if (currentSurfaceNo == 0) {
			SurfaceVector.at(0)->setCold(true);
			sendSurfacePropertiesToController();
		}
	}

	else if (key == 'w') {
		if (currentSurfaceNo == 0) {
			SurfaceVector.at(0)->setCold(false);
			sendSurfacePropertiesToController();
		}
	}

	else if (key == 't') {
		if (currentSurfaceNo == 0) {
			int newTemperature = SurfaceVector.at(0)->getTemperature() - 1;
			SurfaceVector.at(0)->setTemperature(newTemperature);
			sendSurfacePropertiesToController();
		}
	}

	else if (key == 'T') {
		if (currentSurfaceNo == 0) {
			int newTemperature = SurfaceVector.at(0)->getTemperature() + 1;
			SurfaceVector.at(0)->setTemperature(newTemperature);
			sendSurfacePropertiesToController();
		}
	}
	

	else if (key == 'p') 
	{

	}
	else if (key == '0')
	{
		currentSubjectRating[currentSurfaceNo][currentlyDisabledTactileFeature] = 0;

		displayText(100, 1100, 1, 0, 0, SurfaceVector.at(currentSurfaceNo)->getName() + string("  Current rating: ") + string("0"));

	}
	else if (key == '1') 
	{
		currentSubjectRating[currentSurfaceNo][currentlyDisabledTactileFeature] = 1;
		displayText(100, 1100, 1, 0, 0, SurfaceVector.at(currentSurfaceNo)->getName() + string("  Current rating: ") + string("1"));
	}
	else if (key == '2') 
	{
		currentSubjectRating[currentSurfaceNo][currentlyDisabledTactileFeature] = 2;
		displayText(100, 1100, 1, 0, 0, SurfaceVector.at(currentSurfaceNo)->getName() + string("  Current rating: ") + string("2"));
	}
	else if (key == '3')
	{
		currentSubjectRating[currentSurfaceNo][currentlyDisabledTactileFeature] = 3;
		displayText(100, 1100, 1, 0, 0, SurfaceVector.at(currentSurfaceNo)->getName() + string("  Current rating: ") + string("3"));
	}
	else if (key == '4')
	{
		currentSubjectRating[currentSurfaceNo][currentlyDisabledTactileFeature] = -1;
		displayText(100, 1100, 1, 0, 0, SurfaceVector.at(currentSurfaceNo)->getName() + string("  Current rating: ") + string("-1"));
	}
	else if (key == '5')
	{
		currentSubjectRating[currentSurfaceNo][currentlyDisabledTactileFeature] = -2;
		displayText(100, 1100, 1, 0, 0, SurfaceVector.at(currentSurfaceNo)->getName() + string("  Current rating: ") + string("-2"));
	}
	else if (key == '6')
	{
		currentSubjectRating[currentSurfaceNo][currentlyDisabledTactileFeature] = -3;
		displayText(100, 1100, 1, 0, 0, SurfaceVector.at(currentSurfaceNo)->getName() + string("  Current rating: ") + string("-3"));
	}

	//rotate around x-axis
	else if (key == 'r') {
		
		SurfaceVector.at(currentSurfaceNo)->rotateXRight();

		mouse->setBallPos(SurfaceVector.at(currentSurfaceNo)->getAngleX());

		glutPostRedisplay();
	}
	else if (key == 'R') {
		
		SurfaceVector.at(currentSurfaceNo)->rotateXLeft();

		mouse->setBallPos(SurfaceVector.at(currentSurfaceNo)->getAngleX());

		glutPostRedisplay();
	}

	else if (key == 'a') 
	{
		//int surfaceNo = currentSurfaceNo;
		//cout << "##########################Current surface: " << surfaceNo << endl;

		if (surfaceNo > 0) 
		{
			surfaceNo--;
			
		}
		updateSurface(surfaceNo);

	}
	else if (key == 'd')
	{
		//int surfaceNo = currentSurfaceNo;
		//cout << "##########################Current surface: " << surfaceNo << endl;
		if (surfaceNo < (NUMBER_OF_SURFACES - 1))
		{
			surfaceNo++;
			//cout << "Current surface: " << surfaceNo;
		}
		updateSurface(surfaceNo);
	}
}

void displayText(float x, float y, int r, int g, int b, string str)
{

	// add label to front layer of camera
	camera->m_frontLayer->addChild(label);
	// assign color to label
	label->m_fontColor.setRed();
	// assign text to label
	label->setText(str);
	// position widget on screen
	label->setLocalPos(x, y);
	glutPostRedisplay();
}

//---------------------------------------------------------------------------

void close(void)
{
    // stop the simulation
    simulationRunning = false;

    // wait for graphics and haptics loops to terminate
    while (!simulationFinished) { cSleepMs(100); }

	delete audioDevice;
	//delete mFileManager;
}

//---------------------------------------------------------------------------

void graphicsTimer(int data)
{
    if (simulationRunning)
    {
        glutPostRedisplay();
    }

    glutTimerFunc(50, graphicsTimer, 0);
}

//---------------------------------------------------------------------------

void updateGraphics(void)
{
	/////////////////////////////////////////////////////////////////////
	// UPDATE DEFORMABLE MODELS
	/////////////////////////////////////////////////////////////////////

	// update skins deformable objects
	//defWorld->updateSkins(mSurfaceMaterials.at(0).isDeformable);


	/////////////////////////////////////////////////////////////////////
	// RENDER SCENE
	/////////////////////////////////////////////////////////////////////

	// update shadow maps (if any)
	world->updateShadowMaps(false, mirroredDisplay);

	// render world
	camera->renderView(windowW, windowH);

	// swap buffers
	glutSwapBuffers();

	// wait until all GL commands are completed
	glFinish();

	// check for any OpenGL errors
	GLenum err;
	err = glGetError();
	if (err != GL_NO_ERROR) cout << "Error: " << gluErrorString(err) << endl;
}

void handlePassiveMouseMotion(int x, int y)
{
	pos currentPos;
	currentPos.x1 = x;
	currentPos.y1 = y;

	if (!mouse->isSingleMouse()) 
	{

		if ((x - (BALL_DIST*(windowH / SURFACE_LENGTH_X))) < 0) 
		{
			currentPos.x2 = 0;
		}
		else
		{
			currentPos.x2 = x - (BALL_DIST*(windowW / SURFACE_LENGTH_X));
		}

		currentPos.y2 = y;
	}

	mouse->setCurrentPos(currentPos);

	mouse->insertPosition(currentPos);

	mouse->computeVelocity();

	if (mouse->isSingleMouse()) 
	{
		mouse->predictMousePosSingle();
	}
	else
	{
		mouse->predictMousePosDouble();
	}

	if (mouse->hasMouseMoved()) 
	{
		//cout << mouse->getCurrentPos().y1 << endl;
		int currentPixelValue[2] = {0, 0};


		if (wrongOrigin)
		{
			if (mouse->isSingleMouse()) 
			{
				int posX = mouse->getPredictedPos().x1;
				int posY = mouse->getPredictedPos().y1;
				//currentPixelValue[0] = pixels[((mouse->getCurrentPos().y1) * tga_res_x) + mouse->getCurrentPos().x1 + 1];
				if (posX < 0) posX = 0;
				if (posY < 0) posY = 0;
				if (posX >= tga_res_x) posX = tga_res_x;
				if (posX >= tga_res_y) posY = tga_res_y;

				int tmpPixel = ((tga_res_y - posY) * tga_res_x - 1) + posX - 1;
				if (tmpPixel < 1) tmpPixel = 1;
				if (tmpPixel > (CwindowW*CwindowH) -1) tmpPixel = (CwindowW*CwindowH)-1;
				currentPixelValue[0] = pixels[tmpPixel];
				//cout << "Y: " << mouse->getCurrentPos().y1 << "   X: " << mouse->getCurrentPos().x1  << endl; // 
			}
			else 
			{
				currentPixelValue[0] = pixels[((tga_res_y - mouse->getPredictedPos().y1) * tga_res_x) + mouse->getPredictedPos().x1 + 1];
				currentPixelValue[1] = pixels[((tga_res_y - mouse->getPredictedPos().y2) * tga_res_x) + mouse->getPredictedPos().x2 + 1];
			}
		}
		else 
		{
			if (mouse->isSingleMouse())
			{
				currentPixelValue[0] = pixels[(mouse->getPredictedPos().y1 * tga_res_x) + mouse->getPredictedPos().x1 + 1];
			}
			else 
			{
				currentPixelValue[0] = pixels[(mouse->getPredictedPos().y1 * tga_res_x) + mouse->getPredictedPos().x1 + 1];
				currentPixelValue[1] = pixels[(mouse->getPredictedPos().y2 * tga_res_x) + mouse->getPredictedPos().x2 + 1];
			}
		}

		if (debug_flag == 1) 
		{
			cout << "mouse pos x: " << x << "\t";
			cout << "mouse pos y: " << y << endl;
			cout << "pixel value: " << std::to_string(currentPixelValue[0]) << endl;
		}

		int currentAngle[2] = { currentPixelValue[0], currentPixelValue[1] };

		if (lastPixelValue[0] != currentPixelValue[0]) {


			//friction
			char bufferF[] = { " FRC#0\n" };
			if (currentlyDisabledTactileFeature != 2)
				bufferF[5] = '0' + SurfaceVector.at(currentSurfaceNo)->getFriction();

			mSerial->WriteData(bufferF, 6);
			//#if (debug_flag == 2) {
			//	cout << "Current Angle 1: " << currentAngle[0] << endl;
			//}

			char buffer[] = { "BUMP1#1\n" };
			if (currentlyDisabledTactileFeature != 4)
				buffer[6] = '0' + currentAngle[0];

			mSerial->WriteData(buffer, 7);
			//cout << buffer << endl;
			if (useTcm2)
			{
				char buffer2[] = { "BUMP2#1\n" };
				if (currentlyDisabledTactileFeature != 4)
					buffer2[6] = '0' + currentAngle[0];

				mSerial->WriteData(buffer2, 7);
				//cout << buffer2 << endl;
			}

			lastPixelValue[0] = currentPixelValue[0];
		}	

		if (!mouse->isSingleMouse() && (lastPixelValue[1] != currentPixelValue[1])) 
		{
			
			if (debug_flag == 2) {
				cout << "Current Angle 2: " << currentAngle[1] << endl;
			}

			char buffer[] = { "BUMP2#1\n" };
			if (currentlyDisabledTactileFeature != 4)
				buffer[6] = '0' + currentAngle[1];

			mSerial->WriteData(buffer, 7);
			lastPixelValue[1] = currentPixelValue[1];
		}
	}

	if (ballCreated)
	{

		mouse->setBallPos(SurfaceVector.at(currentSurfaceNo)->getAngleX());

		//redraw screen
		glutPostRedisplay();
	}

	if (mouse->getVelocity() > mouse->MIN_MOUSE_VEL)
	{
		if (currentlyDisabledTactileFeature != 5)
		{
			

			float gain = AUDIO_GAIN * mouse->getVelocity() / 100;
			if(gain > 1.0f) gain = 1.0f;
			//cout << "############ GAIN: " << gain << endl;
			audioSource->setGain(gain);
		}
		else
			audioSource->setGain(0);
	}
	else if ((mouse->getVelocity() < mouse->MIN_MOUSE_VEL)) 
	{
		audioSource->setGain(0);
	}

	mouse->setLastPos(currentPos);
}

void initSerialCommunication() 
{
	if (useTcm2)
		mSerial = new MySerial(SERIAL_COM_PORT_TCM2);
	else
		mSerial = new MySerial(SERIAL_COM_PORT);
	mSerialTeleOp = new MySerial(SERIAL_COM_PORT_TELEOPERATION);    // adjust as needed


	if (mSerial->IsConnected())
		printf("We're connected with Mouse Controller!");

	//if (mSerialTeleOp->IsConnected())
	//	printf("We're connected for TeleOperation as well!");

	char incomingData[256] = "";
}

void printBasicOutput() {
	cout << endl;
	cout << "-----------------------------------" << endl;
	cout << "CHAI3D" << endl;
	cout << "Demo: 50-GEL-membrane" << endl;
	cout << "Copyright 2003-2016" << endl;
	cout << "-----------------------------------" << endl << endl << endl;
	cout << "Keyboard Options:" << endl << endl;
	cout << "[s] - Show/Hide GEL Skeleton" << endl;
	cout << "[m] - Enable/Disable vertical mirroring" << endl;
	cout << "[x] - Exit application" << endl;
	cout << endl << endl;
}

void setGlutSettings(int argc, char* argv[]) {

	// initialize GLUT
	glutInit(&argc, argv);

	// retrieve  resolution of computer display and position window accordingly
	/*screenW = glutGet(GLUT_SCREEN_WIDTH);
	screenH = glutGet(GLUT_SCREEN_HEIGHT);
	windowW = 0.9 * screenH;
	windowH = 0.7 * screenH;
	windowPosY = (screenH - windowH) / 2;
	windowPosX = windowPosY;*/

	windowH = CwindowH;
	windowW = CwindowW;

	cout << "W: " << windowW << " H: " << windowH << endl;

	// initialize the OpenGL GLUT window
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(windowW, windowH);

	if (stereoMode == C_STEREO_ACTIVE)
		glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_STEREO);
	else
		glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

	// create display context and initialize GLEW library
	glutCreateWindow(argv[0]);

#ifdef GLEW_VERSION
	// initialize GLEW
	glewInit();
#endif

	// setup GLUT options
	glutDisplayFunc(updateGraphics);
	glutKeyboardFunc(keySelect);
	glutReshapeFunc(resizeWindow);
	glutSetWindowTitle("CHAI3D");
	glutPassiveMotionFunc(handlePassiveMouseMotion);

	// set fullscreen mode
	if (fullscreen)
	{
		glutFullScreen();
	}
}

int setupAudio() {

	//--------------------------------------------------------------------------
	// SETUP AUDIO
	//--------------------------------------------------------------------------

	if (audioSource != NULL) {
		audioSource->stop();
	}

	audioBuffer = audioDevice->newAudioBuffer();

	bool fileload = audioBuffer->loadFromFile(SurfaceVector.at(currentSurfaceNo)->getSoundPath());

	if (!fileload)
	{
		cout << "Error - Sound file failed to load or initialize correctly." << endl;
		close();
		return (-1);
	}

	//cout << "new audio buffer: " << audioBuffer->getFilename() << endl;

	// create audio source
	audioSource = audioDevice->newAudioSource();

	// assign auio buffer to audio source
	audioSource->setAudioBuffer(audioBuffer);

	// loop playing of sound
	audioSource->setLoop(true);

	// turn off sound for now
	audioSource->setGain(0.0);

	// set pitch
	audioSource->setPitch(1.0);

	// play sound
	audioSource->play();

	return 0;
}

void createMouse() {

	mouse = new Mouse(world, true, windowW, windowH);
	glutSetCursor(GLUT_CURSOR_NONE);
	mouse->addToWorld();
	mouse->singleMouseOnOff(true);
}

void loadSurfaces() {

	vector<string> names;
	vector<string> picturePath;
	vector<string> heightMapPath;
	vector<string> soundPath;
	vector<int> temperature;
	vector<bool> cold;
	vector<int> friction;
	vector<int> hardness;
	vector<int> macroStrength;
	vector<int> slipStick;



	names.push_back("G1RhombAluminumMesh");
	names.push_back("G2Brick");
	names.push_back("G2CrushedRock");
	names.push_back("G3StainlessSteel");
	names.push_back("G4CompressedWoodVersion1");
	names.push_back("G4ProfiledWoodPlate");
	names.push_back("G4WoodTypeSilverOak");
	names.push_back("G5ProfiledRubberPlate");
	names.push_back("G7CoarseFoam");
	names.push_back("G8Sandpaper");



	picturePath.push_back(string(("./resources/images/display/G1RhombAluminumMesh.JPG")));
	picturePath.push_back(string(("./resources/images/display/G2Brick.jpg")));
	picturePath.push_back(string(("./resources/images/display/G2CrushedRock.JPG")));
	picturePath.push_back(string(("./resources/images/display/G3StainlessSteel.JPG")));
	picturePath.push_back(string(("./resources/images/display/G4CompressedWoodVersion1.JPG")));
	picturePath.push_back(string(("./resources/images/display/G4ProfiledWoodPlate.JPG")));
	picturePath.push_back(string(("./resources/images/display/G4WoodTypeSilverOak.JPG")));
	picturePath.push_back(string(("./resources/images/display/G5ProfiledRubberPlate.JPG")));
	picturePath.push_back(string(("./resources/images/display/G7CoarseFoam.JPG")));
	picturePath.push_back(string(("./resources/images/display/G8Sandpaper.JPG")));



	//////    D:\Lehre\Computational_haptics_lab\SS_2016\chai3d-3.1.0\modules\GEL\bin\resources\images\heightMaps

	heightMapPath.push_back(string(("./resources/images/heightMaps/G1RhombAluminumMesh.tga")));
	heightMapPath.push_back(string(("./resources/images/heightMaps/G2Brick.tga")));
	heightMapPath.push_back(string(("./resources/images/heightMaps/G2CrushedRock.tga")));
	heightMapPath.push_back(string(("./resources/images/heightMaps/G3StainlessSteel.tga")));
	heightMapPath.push_back(string(("./resources/images/heightMaps/G4CompressedWoodVersion1.tga")));
	heightMapPath.push_back(string(("./resources/images/heightMaps/G4ProfiledWoodPlate.tga")));
	heightMapPath.push_back(string(("./resources/images/heightMaps/G4WoodTypeSilverOak.tga")));
	heightMapPath.push_back(string(("./resources/images/heightMaps/G5ProfiledRubberPlate.tga")));
	heightMapPath.push_back(string(("./resources/images/heightMaps/G7CoarseFoam.tga")));
	heightMapPath.push_back(string(("./resources/images/heightMaps/G8Sandpaper.tga")));





	soundPath.push_back(string(("./resources/acceleration/G1RhombAluminumMesh.wav")));
	soundPath.push_back(string(("./resources/acceleration/G2Brick.wav")));
	soundPath.push_back(string(("./resources/acceleration/G2CrushedRock.wav")));
	soundPath.push_back(string(("./resources/acceleration/G3StainlessSteel.wav")));
	soundPath.push_back(string(("./resources/acceleration/G4CompressedWoodVersion1.wav")));
	soundPath.push_back(string(("./resources/acceleration/G4ProfiledWoodenPlate.wav")));
	soundPath.push_back(string(("./resources/acceleration/G4WoodTypeSilverOak.wav")));
	soundPath.push_back(string(("./resources/acceleration/G5ProfiledRubber.wav")));
	soundPath.push_back(string(("./resources/acceleration/G7CoarseFoam.wav")));
	soundPath.push_back(string(("./resources/acceleration/G8Sandpaper.wav")));

	

	// G1RhombAluminumMesh,  G2Brick,  G2CrushedRock,  G3StainlessSteel, G4CompressedWood, G4ProfiledWoodPlate, G4WoodTypeSilverOak, G5ProfiledRubberPlate,  G7CoarseFoam, G8Sandpaper

	temperature.push_back(2);
	temperature.push_back(3);
	temperature.push_back(3);
	temperature.push_back(4);
	temperature.push_back(0);
	temperature.push_back(0);
	temperature.push_back(0);
	temperature.push_back(0);
	temperature.push_back(0);
	temperature.push_back(0);



	if (switchWarmCold)
	{
		// G1RhombAluminumMesh,  G2Brick,  G2CrushedRock,  G3StainlessSteel, G4CompressedWood, G4ProfiledWoodPlate, G4WoodTypeSilverOak, G5ProfiledRubberPlate,  G7CoarseFoam, G8Sandpaper
		cold.push_back(false);
		cold.push_back(false);
		cold.push_back(true);
		cold.push_back(false);
		cold.push_back(true);
		cold.push_back(true);
		cold.push_back(true);
		cold.push_back(true);
		cold.push_back(true);
		cold.push_back(true);
	}
	else
	{
		cold.push_back(true);
		cold.push_back(true);
		cold.push_back(false);
		cold.push_back(true);
		cold.push_back(false);
		cold.push_back(false);
		cold.push_back(false);
		cold.push_back(false);
		cold.push_back(false);
		cold.push_back(false);
	}





	// G1RhombAluminumMesh,  G2Brick,  G2CrushedRock,  G3StainlessSteel, G4CompressedWood, G4ProfiledWoodPlate, G4WoodTypeSilverOak, G5ProfiledRubberPlate,  G7CoarseFoam, G8Sandpaper	
	friction.push_back(5);
	friction.push_back(4);
	friction.push_back(5);
	friction.push_back(0);
	friction.push_back(4);
	friction.push_back(5);
	friction.push_back(4);
	friction.push_back(6);
	friction.push_back(7);
	friction.push_back(5);


	// G1RhombAluminumMesh,  G2Brick,  G2CrushedRock,  G3StainlessSteel, G4CompressedWood, G4ProfiledWoodPlate, G4WoodTypeSilverOak, G5ProfiledRubberPlate,  G7CoarseFoam, G8Sandpaper

	hardness.push_back(0);
	hardness.push_back(0);
	hardness.push_back(0);
	hardness.push_back(0);
	hardness.push_back(0);
	hardness.push_back(0);
	hardness.push_back(0);

	hardness.push_back(4);
	hardness.push_back(8);
	hardness.push_back(2);


	// G1RhombAluminumMesh,  G2Brick,  G2CrushedRock,  G3StainlessSteel, G4CompressedWood, G4ProfiledWoodPlate, G4WoodTypeSilverOak, G5ProfiledRubberPlate,  G7CoarseFoam, G8Sandpaper

	macroStrength.push_back(4);
	macroStrength.push_back(0);
	macroStrength.push_back(4);
	macroStrength.push_back(0);
	macroStrength.push_back(2);
	macroStrength.push_back(4);
	macroStrength.push_back(2);
	macroStrength.push_back(3);
	macroStrength.push_back(4);
	macroStrength.push_back(0);
	
	// G1RhombAluminumMesh,  G2Brick,  G2CrushedRock,  G3StainlessSteel, G4CompressedWood, G4ProfiledWoodPlate, G4WoodTypeSilverOak, G5ProfiledRubberPlate,  G7CoarseFoam, G8Sandpaper

	slipStick.push_back(2);
	slipStick.push_back(1);
	slipStick.push_back(2);
	slipStick.push_back(1);
	slipStick.push_back(1);
	slipStick.push_back(3);
	slipStick.push_back(1);
	slipStick.push_back(4);
	slipStick.push_back(3);
	slipStick.push_back(1);





	for (int i = 0; i < NUMBER_OF_SURFACES; i++) 
	{
		cout << i << endl;
		Surface* surface = new Surface((i == 0), names.at(i), world, picturePath.at(i), heightMapPath.at(i), soundPath.at(i), temperature.at(i), cold.at(i), friction.at(i), hardness.at(i), macroStrength.at(i), slipStick.at(i));
		SurfaceVector.push_back(surface);
	}
	cout << "loading surfaces finished..." << endl;
}

void updateSurface(int surfaceNo)
{
	cout << endl << endl;
	//cout << "old surface: " << SurfaceVector.at(currentSurfaceNo)->getName() << ", new surface: " << SurfaceVector.at(surfaceNo)->getName() << endl;

	double currentSurfaceAngleX = SurfaceVector.at(currentSurfaceNo)->getAngleX();

	SurfaceVector.at(currentSurfaceNo)->removeFromWorld();


	if (!useExperimentalPermutation)
	{
		currentSurfaceNo = surfaceNo;
	}
	else
	{
		currentSurfaceNo = experimentPermutationVector.at(surfaceNo);
	}
	

	SurfaceVector.at(currentSurfaceNo)->addToWorld(enableVisual);
	SurfaceVector.at(currentSurfaceNo)->rotateX(currentSurfaceAngleX);

	pixels = SurfaceVector.at(currentSurfaceNo)->getHeightMapPixels();

	setupAudio();

	sendSurfacePropertiesToController();

	int e = 4; //)
	string s = SurfaceVector.at(currentSurfaceNo)->getName() + string("    Current rating: ") + toStrVal(currentSubjectRating[currentSurfaceNo][currentlyDisabledTactileFeature]);
	displayText(100, 1100, 1, 0, 0, s);


	
	glutPostRedisplay();
}

void sendSurfacePropertiesToController()
{

	//temperature
	if (SurfaceVector.at(currentSurfaceNo)->isCold()) 
	{
		char buffer[] = { " COLD#0\n" };
		if(currentlyDisabledTactileFeature != 1)
			buffer[6] = '0' + SurfaceVector.at(currentSurfaceNo)->getTemperature();

		//cout << "COLD: " << buffer << endl;
		mSerial->WriteData(buffer, 7);
	}
	else 
	{
		char buffer[] = { " HOT#0\n" };
		if (currentlyDisabledTactileFeature != 1)
			buffer[5] = '0' + SurfaceVector.at(currentSurfaceNo)->getTemperature();

		//cout << "HOT: " << buffer << endl;
		mSerial->WriteData(buffer, 6);
	}

	//slipstick
	if (useStickSlip)
	{

		char bufferSlip[] = { " SLP#0\n" };
		if (currentlyDisabledTactileFeature != 2)
			bufferSlip[5] = '0' + SurfaceVector.at(currentSurfaceNo)->getSlipStick();

		mSerial->WriteData(bufferSlip, 6);
	}

	//friction
	char buffer[] = { " FRC#0\n" };
	if (currentlyDisabledTactileFeature != 2)
		buffer[5] = '0' + SurfaceVector.at(currentSurfaceNo)->getFriction();

	mSerial->WriteData(buffer, 6);
	//cout << "FRIC: " << buffer << endl;

	// hardness
	char bufferH[] = { " HARDN#1\n" };		
	char bufferS[] = { " SOFTN#1\n" };

	bufferH[7] = '0' + SurfaceVector.at(currentSurfaceNo)->getHardness();
	bufferS[7] = '0' + SurfaceVector.at(currentSurfaceNo)->getHardness();
	

	int decision = SurfaceVector.at(currentSurfaceNo)->getHardness();
	if (decision > 0)
	{
		if (currentlyDisabledTactileFeature == 3)
		{
			mSerial->WriteData(bufferH, 8);
		}
		else
		{
			mSerial->WriteData(bufferS, 8);
		}
		
	}
	else
	{	// Experiment: switch hardness/softness!!!!
		if (currentlyDisabledTactileFeature == 3)
		{
			mSerial->WriteData(bufferS, 8);
		}
		else
		{
			mSerial->WriteData(bufferH, 8);
		}
		
	}

	//cout << "Hardness: " << bufferH << endl;



	char bufferM[] = { " MACRO#1\n" };
	if (currentlyDisabledTactileFeature != 4)
		bufferM[7] = '0' + SurfaceVector.at(currentSurfaceNo)->getMacroStrength();

	mSerial->WriteData(bufferM, 8);
	//cout << "Macro: " << bufferM << endl;


	//SurfaceVector.at(currentSurfaceNo)->printTextOutput();



}

void create3DSceneGraph() 
{

	// create a camera and insert it into the virtual world
	camera = new cCamera(world);
	world->addChild(camera);

	currentPos = cVector3d(0.001, 0.0, 10.0);

	// position and orient the camera
	camera->set(currentPos,    // camera position (eye)
		cVector3d(0.0, 0.0, 0.0),    // lookat position (target)
		cVector3d(0.0, 1.0, 0.0));   // direction of the (up) vector

	// set the near and far clipping planes of the camera
	camera->setClippingPlanes(-10.0, 10.0);

	// set stereo mode
	camera->setStereoMode(stereoMode);

	// set stereo eye separation and focal length (applies only if stereo is enabled)
	camera->setStereoEyeSeparation(0.02);
	camera->setStereoFocalLength(3.0);

	// set vertical mirrored display mode
	camera->setMirrorVertical(mirroredDisplay);

	// enable multi-pass rendering to handle transparent objects
	camera->setUseMultipassTransparency(true);

	// create a directional light source
	light = new cDirectionalLight(world);

	// insert light source inside world
	world->addChild(light);

	// enable light source
	light->setEnabled(true);

	// define direction of light beam
	light->setDir(0.0, -1.0, -2.0);
}







