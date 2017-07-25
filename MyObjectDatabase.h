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

/* ##################################################
 => audio file !!!!!!!!!
 
 => adjust properties for different materials

  ################################################*/

//------------------------------------------------------------------------------
// Properties
//------------------------------------------------------------------------------	

/*
Texture, NormalMap, Audio, Size, Orientation, Shape, Temperature, Stiffness, StaticFriction, DynamicFriction, TextureLevel, AudioGain, AudioPitch, AudioPitchOffset);

MyProperties(string setTexture, string setNormalMap, string setAudio, cVector3d setSize, MyOrientation setOrientation, enum MyShape setShape, int setTemperature,
double setStiffness, double setStaticFriction, double setDynamicFriction, double setTextureLevel, double setAudioGain, double setAudioPitchGain, double setAudioPitchOffset);


*/

// set some orientation
struct MyOrientation orientation1 {
	cVector3d(0.0, 0.0, 0.0), 0.0
};

MyProperties Cube_Aluminium("G1RhombAluminumMesh.JPG", "G1RhombAluminumMeshNormal.png", "compressedWood.wav", cVector3d(0.3, 0.3, 0.3), orientation1, MyShape(cube),
	3, 0.6, 0.1, 0.1, 0.1, 1.0, 0.8, 0.8);

MyProperties Sphere_Steel("G3StainlessSteel.JPG", "XXXNormal.png", "compressedWood.wav", cVector3d(0.3, 0.3, 0.3), orientation1, MyShape(sphere),
	3, 0.7, 0.1, 0.1, 0.2, 0.0, 0.2, 0.8);

MyProperties Cylinder_Granite("G2GraniteTypeVeneziano.JPG", "XXXNormal.png", "compressedWood.wav", cVector3d(0.2, 0.2, 0.5), orientation1, MyShape(cylinder),
	3, 0.9, 0.8, 0.8, 0.2, 0.0, 0.2, 0.8);

MyProperties Cube_WoodProfiled("G4ProfiledWoodPlate.JPG", "G4ProfiledWoodPlateNormal.JPG", "compressedWood.wav", cVector3d(0.2, 0.2, 0.5), orientation1, MyShape(cube),
	3, 0.6, 0.4, 0.4, 0.2, 0.8, 0.2, 0.8);


/*
MyProperties Aluminium("G1RhombAluminumMesh.JPG", "G1RhombAluminumMeshNormal.png", 3, 0.5, 0.2, 0.2, 0.2, 0, 0.2);

MyProperties Steel("G3StainlessSteel.JPG", "XXXNormal.png", 3, 0.5, 0.2, 0.2, 0.2, 0, 0.2);

MyProperties Rubber("G5ProfiledRubberPlate.JPG", "XXXNormal.png", 3, 0.5, 0.2, 0.2, 0.2, 0, 0.2);

MyProperties Granite("G2GraniteTypeVeneziano.JPG", "XXXNormal.png", 3, 0.5, 0.2, 0.2, 0.2, 0, 0.2);

MyProperties Rock("G2CrushedRock.JPG", "XXXNormal.png", 3, 0.5, 0.2, 0.2, 0.2, 0, 0.2);

MyProperties WoodCompressed("G4CompressedWoodVersion1.JPG", "XXXNormal.png", 3, 0.5, 0.2, 0.2, 0.2, 0, 0.2);

MyProperties WoodProfiled("G4ProfiledWoodPlate.JPG", "XXXNormal.png", 3, 0.5, 0.2, 0.2, 0.2, 0, 0.2);

MyProperties WoodOak("G4WoodTypeSilverOak.JPG", "XXXNormal.png", 3, 0.5, 0.2, 0.2, 0.2, 0, 0.2);

MyProperties Foam("G7CoarseFoam.JPG", "XXXNormal.png", 3, 0.5, 0.2, 0.2, 0.2, 0, 0.2);
*/