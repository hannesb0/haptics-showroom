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

MyProperties Cube_Aluminium("G1RhombAluminumMesh.JPG", "G1RhombAluminumMeshNormal.JPG", "RhombicAluminumMesh_3_Z.wav", cVector3d(0.3, 0.3, 0.3), orientation1, MyShape(cube),
	3, 0.6, 0.1, 0.1, 0.1, 1.0, 0.8, 0.8);

MyProperties Sphere_Steel("G3StainlessSteel.JPG", "G3StainlessSteelNormal.JPG", "StainlessSteel_5_Y.wav", cVector3d(0.1, 0.1, 0.1), orientation1, MyShape(sphere),
	3, 0.7, 0.1, 0.1, 0.2, 1.0, 0.2, 0.8);

MyProperties Cylinder_Coarsefoam("G7CoarseFoam.JPG", "G7CoarseFoam_NRM.JPG", "CoarseFoam_3_Z.wav", cVector3d(0.2, 0.2, 0.1), orientation1, MyShape(cube),
	3, 0.6, 0.1, 0.1, 0.1, 1.0, 0.8, 0.8);

MyProperties Cube_WoodProfiled("G4ProfiledWoodPlate.JPG", "G4ProfiledWoodPlateNormal.JPG", "ProfiledWoodenPlate_3_Z.wav", cVector3d(0.2, 0.2, 0.4), orientation1, MyShape(cube),
	3, 0.6, 0.4, 0.4, 0.2, 1.0, 0.2, 0.8);

MyProperties Cube_Rubber("G5ProfiledRubberPlate.JPG", "G5ProfiledRubberPlateNormal.JPG", "ProfiledRubberPlate_3_Z.wav", cVector3d(0.2, 0.2, 0.5), orientation1, MyShape(cylinder),
	3, 0.9, 0.8, 0.8, 0.2, 1.0, 0.2, 0.8);

MyProperties Cube_CrushedRock("G2CrushedRock.JPG", "G2CrushedRockNormal.JPG", "CrushedRock_3_Z.wav", cVector3d(0.3, 0.3, 0.3), orientation1, MyShape(cube),
	3, 0.6, 0.1, 0.1, 0.1, 1.0, 0.8, 0.8);

MyProperties Cube_SilverOak("G4WoodTypeSilverOak.JPG", "G4WoodTypeSilverOakNormal.JPG", "CompressedWood_3_Z.wav", cVector3d(0.4, 0.4, 0.4), orientation1, MyShape(cube),
	3, 0.6, 0.1, 0.1, 0.1, 1.0, 0.8, 0.8);

MyProperties Cube_Ice("frost.jpg", "frostNormal.jpg", "", cVector3d(0.4, 0.4, 0.2), orientation1, MyShape(cube),
	1, 0.8, 0.1, 0.1, 0.1, 0.0, 0.8, 0.8);

MyProperties Cube_Lava("heat1.jpg", "heat1Normal.jpg", "", cVector3d(0.4, 0.4, 0.2), orientation1, MyShape(sphere),
	5, 0.8, 0.1, 0.1, 0.1, 0.0, 0.8, 0.8);


//------------------------------------------------------------------------------
// Room environment
//------------------------------------------------------------------------------	

MyProperties myFloor("floor1small.jpg", "", "", cVector3d(roomLength, roomWidth, 0.0), MyOrientation{ cVector3d(0.0, 0.0, 0.0), 0.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myCeiling("Ceiling.jpg", "", "", cVector3d(roomLength, roomWidth, 0.0), MyOrientation{ cVector3d(0.0, 1.0, 0.0), 180.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myFrontWall("sand-wall.png", "", "", cVector3d(roomHeight, roomWidth, 0.0), MyOrientation{ cVector3d(0.0, 1.0, 0.0), -90.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myBackWall("sand-wall.png", "", "", cVector3d(roomHeight, roomWidth, 0.0), MyOrientation{ cVector3d(0.0, 1.0, 0.0), 90.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myLeftWall("sand-wall.png", "", "", cVector3d(roomLength, roomHeight, 0.0), MyOrientation{ cVector3d(1.0, 0.0, 0.0), -90.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myRightWall("sand-wall.png", "", "", cVector3d(roomLength, roomHeight, 0.0), MyOrientation{ cVector3d(1.0, 0.0, 0.0), 90.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myWindow("window1.jpg", "", "", cVector3d(0.75, 1.25, 0.0), MyOrientation{ cVector3d(0.0, 1.0, 0.0), 90.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);

MyProperties myTitle("title.jpg", "", "", cVector3d(0.3, 1.32, 0.0), MyOrientation{ cVector3d(0.0, 1.0, 0.0), 90.0 }, MyShape(plane),
	3, 0.8, 0.1, 0.1, 0.1, 0.0, 0.0, 0.0);











/*
MyProperties Cube_Aluminium("G1RhombAluminumMesh.JPG", "G1RhombAluminumMeshNormal.JPG", "metal-scraping.wav", cVector3d(0.3, 0.3, 0.3), orientation2, MyShape(cube),
3, 0.6, 0.1, 0.1, 0.1, 1.0, 0.8, 0.8);

MyProperties Sphere_Steel("G3StainlessSteel.JPG", "G3StainlessSteelNormal.JPG", "metal-scraping.wav", cVector3d(0.3, 0.3, 0.3), orientation1, MyShape(sphere),
4, 0.7, 0.1, 0.1, 0.2, 1.0, 0.2, 0.8);

MyProperties Cylinder_Granite("G2GraniteTypeVeneziano.JPG", "G2GraniteTypeVenezianoNormal.JPG", "wood-scraping.wav", cVector3d(0.2, 0.2, 0.5), orientation1, MyShape(cylinder),
3, 0.9, 0.8, 0.8, 0.2, 1.0, 0.2, 0.8);

MyProperties Cube_WoodProfiled("G4ProfiledWoodPlate.JPG", "G4ProfiledWoodPlateNormal.JPG", "compressedWood.wav", cVector3d(0.2, 0.2, 0.4), orientation1, MyShape(cube),
2, 0.6, 0.4, 0.4, 0.2, 1.0, 0.2, 0.8);

MyProperties Rubber("G5ProfiledRubberPlate.JPG", "G5ProfiledRubberPlateNormal.JPG", 3, 0.5, 0.2, 0.2, 0.2, 0, 0.2);

MyProperties Rock("G2CrushedRock.JPG", "G2CrushedRockNormal.JPG", 3, 0.5, 0.2, 0.2, 0.2, 0, 0.2);

MyProperties WoodCompressed("G4CompressedWoodVersion1.JPG", "G4CompressedWoodVersion1Normal.JPG", 3, 0.5, 0.2, 0.2, 0.2, 0, 0.2);



MyProperties WoodOak("G4WoodTypeSilverOak.JPG", "G4WoodTypeSilverOakNormal.JPG", 3, 0.5, 0.2, 0.2, 0.2, 0, 0.2);
// ->



MyProperties Foam("G7CoarseFoam.JPG", "G7CoarseFoamNormal.JPG", 3, 0.5, 0.2, 0.2, 0.2, 0, 0.2);
*/
