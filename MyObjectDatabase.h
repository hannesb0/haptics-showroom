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

#include "MyObject.h"

/* ##################################################
 => audio file !!!!!!!!!
 
 => adjust properties for different materials

  ################################################*/

//------------------------------------------------------------------------------
// Material properties
//------------------------------------------------------------------------------	

/*
Texture, NormalMap, Temperature, Stiffness, StaticFriction, DynamicFriction, TextureLevel, AudioGain, AudioPitch);
*/

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

//------------------------------------------------------------------------------
// Objects
//------------------------------------------------------------------------------	

/*
Size, Shape, Properties
*/

//MyObject Cube_Aluminium(cVector3d(0.2, 0.2, 0.2), MyShape(cube), MyProperties(Aluminium));

//MyObject Sphere_Rubber(cVector3d(0.2, 0.2, 0.2), MyShape(sphere), MyProperties(Rubber));

//MyObject Cylinder_Steel(cVector3d(0.2, 0.2, 0.2), MyShape(cylinder), MyProperties(Steel));