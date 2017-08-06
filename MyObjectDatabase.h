//==============================================================================
/*
Filename:	MyObjectDatabase.h
Project:	Haptics Showroom
Authors:	Naina Dhingra, Ke Xu, Hannes Bohnengel
Revision:	0.2
Remarks:	These files are tracked with git and are available on the github
repository: https://github.com/hannesb0/haptics-showroom
*/
//==============================================================================

#include "MyProperties.h"
#include "Global.h"



//------------------------------------------------------------------------------
// Orientations
//------------------------------------------------------------------------------	

// set some orientation
struct MyOrientation orientation1 {
	cVector3d(0.0, 0.0, 0.0), 0.0
};

// set some orientation
struct MyOrientation orientation2 {
	cVector3d(1.0, 1.0, 1.0), 45.0
};

//------------------------------------------------------------------------------
// Objects
//------------------------------------------------------------------------------	

/*
Texture, NormalMap, Audio, Size, Orientation, Shape, 
Temperature, Stiffness, StaticFriction, DynamicFriction, TextureLevel, AudioGain, AudioPitch, AudioPitchOffset);

MyProperties(string setTexture, string setNormalMap, string setAudio, cVector3d setSize, MyOrientation setOrientation, enum MyShape setShape, int setTemperature,
double setStiffness, double setStaticFriction, double setDynamicFriction, double setTextureLevel, double setAudioGain, double setAudioPitchGain, double setAudioPitchOffset);

Texture:	file name for displaying texture
NormalMap:	
Audio:		
...

*/

MyProperties Cube_Aluminium("G1RhombAluminumMesh.JPG", "G1RhombAluminumMeshNormal.JPG", "RhombicAluminumMesh_3_Z.wav", cVector3d(0.5, 0.35, 0.01), orientation1, MyShape(cube),
	3, 0.8, 0.3, 0.3, 0.2, 1.0, 0.2, 0.8);

MyProperties Cube_SilverOak("G4WoodTypeSilverOak.JPG", "G4WoodTypeSilverOakNormal.JPG", "CompressedWood_3_Z.wav", cVector3d(0.5, 0.35, 0.01), orientation1, MyShape(cube),
	3, 0.6, 0.2, 0.2, 0.1, 1.0, 0.2, 0.8);

MyProperties Cube_CrushedRock("G2CrushedRock.JPG", "G2CrushedRockNormal.JPG", "CrushedRock_3_Z.wav", cVector3d(0.5, 0.35, 0.01), orientation1, MyShape(cube),
	3, 1.0, 0.1, 0.1, 0.1, 1.0, 0.2, 0.8);

MyProperties Cube_Steel("G3StainlessSteel.JPG", "G3StainlessSteelNormal.JPG", "StainlessSteel_5_Y.wav", cVector3d(0.5, 0.35, 0.01), orientation1, MyShape(cube),
	3, 0.8, 0.1, 0.1, 0.2, 1.0, 0.2, 0.8);

MyProperties Cube_WoodProfiled("G4ProfiledWoodPlate.JPG", "G4ProfiledWoodPlateNormal.JPG", "ProfiledWoodenPlate_3_Z.wav", cVector3d(0.5, 0.35, 0.01), orientation1, MyShape(cube),
	3, 0.6, 0.3, 0.3, 0.2, 1.0, 0.2, 0.8);

MyProperties Cube_Rubber("G5ProfiledRubberPlate.JPG", "G5ProfiledRubberPlateNormal.JPG", "ProfiledRubberPlate_3_Z.wav", cVector3d(0.5, 0.35, 0.01), orientation1, MyShape(cube),
	3, 0.6, 0.9, 0.9, 0.3, 1.0, 0.2, 0.8);


MyProperties Cube_GlowingMetal("heat1.jpg", "", "", cVector3d(0.5, 0.35, 0.01), orientation1, MyShape(cube),
	5, 0.7, 0.5, 0.5, 0.01, 0.0, 0.0, 0.0);

MyProperties Cube_Lava("lava.jpg", "lavaNormal.jpg", "", cVector3d(0.5, 0.35, 0.01), orientation1, MyShape(cube),
	4, 0.5, 0.5, 0.5, 0.01, 0.0, 0.0, 0.0);

MyProperties Cube_Granite("G2GraniteTypeVeneziano.jpg", "", "", cVector3d(0.5, 0.35, 0.01), orientation1, MyShape(cube),
	2, 1.0, 0.01, 0.01, 0.01, 0.0, 0.0, 0.0);

MyProperties Cube_Ice("frost.jpg", "frostNormal.jpg", "", cVector3d(0.5, 0.35, 0.01), orientation1, MyShape(cube),
	1, 0.8, 0.01, 0.01, 0.01, 0.0, 0.0, 0.0);

/*
MyProperties Cube_Coarsefoam("G7CoarseFoam.JPG", "G7CoarseFoam_NRM.JPG", "CoarseFoam_3_Z.wav", cVector3d(0.2, 0.2, 0.1), orientation1, MyShape(cube),
	3, 0.6, 0.1, 0.1, 0.1, 1.0, 0.8, 0.8);
*/

MyProperties Property_3ds("whiteWall.jpg", "whiteWallNormal.png", "", cVector3d(0.2, 0.2, 0.4), orientation1, MyShape(cube),
	3, 0.6, 0.4, 0.4, 0.2, 0.0, 0.2, 0.8);


//------------------------------------------------------------------------------
// Room environment
//------------------------------------------------------------------------------	

MyProperties myFloor("floor1small.jpg", "", "", cVector3d(roomLength, roomWidth, 0.0), MyOrientation{ cVector3d(0.0, 0.0, 0.0), 0.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myCeiling("ceiling1.jpg", "", "", cVector3d(roomLength, roomWidth, 0.0), MyOrientation{ cVector3d(0.0, 1.0, 0.0), 180.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myFrontWall("whiteWall.jpg", "", "", cVector3d(roomHeight, roomWidth, 0.0), MyOrientation{ cVector3d(0.0, 1.0, 0.0), -90.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myBackWall("whiteWall.jpg", "", "", cVector3d(roomHeight, roomWidth, 0.0), MyOrientation{ cVector3d(0.0, 1.0, 0.0), 90.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myLeftWall("whiteWall.jpg", "", "", cVector3d(roomLength, roomHeight, 0.0), MyOrientation{ cVector3d(1.0, 0.0, 0.0), -90.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myRightWall("whiteWall.jpg", "", "", cVector3d(roomLength, roomHeight, 0.0), MyOrientation{ cVector3d(1.0, 0.0, 0.0), 90.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myWindow("window1.jpg", "", "", cVector3d(0.6, 0.8, 0.0), MyOrientation{ cVector3d(0.0, 1.0, 0.0), 90.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);


//------------------------------------------------------------------------------
// Labels
//------------------------------------------------------------------------------	
#if 1
MyProperties myTitle("title.jpg", "", "", cVector3d(0.3, 1.32, 0.0), MyOrientation{ cVector3d(0.0, 1.0, 0.0), 90.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myLeftLablePW("ProfiledWood.png", "", "", cVector3d(0.615, 0.15, 0.0), MyOrientation{ cVector3d(1.0, 0.0, 0.0), -90.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myLeftLableSS("StainlessSteel.png", "", "", cVector3d(0.615, 0.15, 0.0), MyOrientation{ cVector3d(1.0, 0.0, 0.0), -90.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myLeftLableCR("CrushedRock.png", "", "", cVector3d(0.615, 0.15, 0.0), MyOrientation{ cVector3d(1.0, 0.0, 0.0), -90.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myLeftLableSO("SilverOak.png", "", "", cVector3d(0.615, 0.15, 0.0), MyOrientation{ cVector3d(1.0, 0.0, 0.0), -90.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myLeftLableAM("AluminiumMesh.png", "", "", cVector3d(0.615, 0.15, 0.0), MyOrientation{ cVector3d(1.0, 0.0, 0.0), -90.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myRightLablePR("ProfiledRubber.png", "", "", cVector3d(0.615, 0.15, 0.0), MyOrientation{ cVector3d(1.0, 0.0, 0.0), 90.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myRightLableGS("GlowingSteel.png", "", "", cVector3d(0.615, 0.15, 0.0), MyOrientation{ cVector3d(1.0, 0.0, 0.0), 90.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myRightLableSL("StiffenedLava.png", "", "", cVector3d(0.615, 0.15, 0.0), MyOrientation{ cVector3d(1.0, 0.0, 0.0), 90.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myRightLableCG("ColdGranite.png", "", "", cVector3d(0.615, 0.15, 0.0), MyOrientation{ cVector3d(1.0, 0.0, 0.0), 90.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myRightLableFW("FrozenWater.png", "", "", cVector3d(0.615, 0.15, 0.0), MyOrientation{ cVector3d(1.0, 0.0, 0.0), 90.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myRightLableCaution("CautionHot.jpg", "", "", cVector3d(0.3, 0.22, 0.0), MyOrientation{ cVector3d(1.0, 0.0, 0.0), 90.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

#endif