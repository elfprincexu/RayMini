// *********************************************************
// Scene Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Scene.h"
using namespace std;

static Scene * instance = NULL;

Scene * Scene::getInstance () {
    if (instance == NULL)
        instance = new Scene ();
    return instance;
}

void Scene::destroyInstance () {
    if (instance != NULL) {
        delete instance;
        instance = NULL;
    }
}

Scene::Scene ()
    :   m_pointCloud() {
    ParameterHandler* params = ParameterHandler::Instance();
   
    kdTree = NULL;
    int scene = params -> GetScene();
    
    if(scene == 0){
        buildDefaultScene ();
    }
    else if(scene == 1){
        buildSphereScene ();
    }
    else if(scene == 2){
        buildRoomScene();
    }
    else if(scene == 3){
        buildBMWScene();
    }
    updateBoundingBox ();
}

Scene::~Scene () {
    m_pointCloudBuilt = false;
    if ( kdTree ) {
        delete kdTree;
        kdTree = (KdTree*)0x0;
    }
}

void Scene::buildKdTree () {
    KdDataVector ktData;
    const std::vector< Object >& sceneObjects = getObjects ();

    for ( unsigned int obj = 0; obj < sceneObjects.size (); obj++ ) {
        const Object& object = sceneObjects[obj];
        const std::vector< Triangle >& objectTriangles = object.getMesh ().getTriangles ();

        for ( unsigned int tri = 0; tri < objectTriangles.size (); tri++ ) {
            ktData.push_back ( new KdData ( tri, &object ) );
        }
    }

    kdTree = new KdTree ( ktData );
}

void Scene::updateBoundingBox () {
    if (objects.empty ())
        bbox = BoundingBox ();
    else {
        bbox = objects[0].getBoundingBox ();
        for (unsigned int i = 1; i < objects.size (); i++)
            bbox.extendTo (objects[i].getBoundingBox ());
    }
}

// Changer ce code pour creer des scenes originales
void Scene::buildDefaultScene () {
    
    Mesh groundMesh;
    groundMesh.loadOFF ("models/ground.off");
    Mesh tesGroundMesh;
    groundMesh.Tesselate (0.01f, tesGroundMesh);
    Material groundMat;
    //groundMat.setDiffuse(0.0f);
    //groundMat.setSpecular(1.0f);
    //groundMat.setShininess(1.0f);
    Object ground (tesGroundMesh, groundMat);    
    objects.push_back (ground);
    
    Mesh ramMesh;
    ramMesh.loadOFF ("models/ram.off");
    Mesh tesRamMesh;
    ramMesh.Tesselate (0.01f, tesRamMesh);
    Material ramMat (1.f, 1.f, Vec3Df (1.f, .6f, .2f));
    Object ram (tesRamMesh, ramMat);
    ram.setTrans (Vec3Df (1.f, 0.5f, 0.f));
    ram.setBumpLevel(0.1f);
    objects.push_back (ram);

    Mesh rhinoMesh;
    rhinoMesh.loadOFF ("models/rhino.off");
    Material rhinoMat (1.0f, 0.2f, Vec3Df (0.6f, 0.6f, 0.7f));
    Object rhino (rhinoMesh, rhinoMat);
    rhino.setTrans (Vec3Df (-1.f, -1.0f, 0.45f));
    objects.push_back (rhino);

    Mesh gargMesh;
    gargMesh.loadOFF ("models/gargoyle.off");
    Material gargMat (0.7f, 0.0f, Vec3Df (0.5f, 0.8f, 0.5f));
    Object garg (gargMesh, gargMat);
    garg.setTrans (Vec3Df (-1.f, 1.0f, 0.1f));
    objects.push_back (garg);

    lights.push_back ( Light (Vec3Df (  3.0f,  3.0f, 3.0f), Vec3Df (1.0f, 1.0f, 1.0f), 0.5f) );
    lights.push_back ( Light (Vec3Df ( -3.0f,  3.0f, 6.0f), Vec3Df (1.0f, 1.0f, 1.0f), 0.5f) );
    lights.push_back ( Light (Vec3Df (  3.0f, -3.0f, 3.0f), Vec3Df (1.0f, 1.0f, 1.0f), 0.5f) );
    lights.push_back ( Light (Vec3Df ( -3.0f, -3.0f, 6.0f), Vec3Df (1.0f, 1.0f, 1.0f), 0.5f) );
    
    //lights.push_back (
    //    new RectangleLight (
    //        0.5f * Vec3Df ( 1.0f, 0.0f, 0.0f ),
    //        0.5f * Vec3Df ( 0.0f, 1.0f, 0.0f ),
    //        Vec3Df ( 0.0f, 3.0f, 3.0f ),
    //        Vec3Df ( 1.0f, 1.0f, 1.0f ),
    //        0.5f
    //    )
    //);
}

// Changer ce code pour creer des scenes originales
void Scene::buildCubeScene () {
    
    Mesh cubeMesh;
    cubeMesh.loadOFF ("models/smallcube.off");
    Material cubeMat (0.7f, 0.4f, Vec3Df (0.5f, 0.8f, 0.5f));
    Object cube (cubeMesh, cubeMat);

    for(float x = -10; x < 10; x = x + 5)
    cube.setTrans (Vec3Df (x, 1.0f, 0.1f));
    objects.push_back (cube);

    Light l (Vec3Df (3.0f, 3.0f, 8.0f), Vec3Df (1.0f, 1.0f, 1.0f), 1.0f);
    lights.push_back (l);

    l = Light (Vec3Df (3.0f, 3.0f, -8.0f), Vec3Df (1.0f, 1.0f, 1.0f), 1.0f);
    lights.push_back (l);

    l = Light (Vec3Df (3.0f, 3.0f, 3.0f), Vec3Df (1.0f, 1.0f, 1.0f), 1.0f);
    lights.push_back (l);


}

/* Group Scene 1: room with cubes */
void Scene::buildRoom () {

    /* BackWall */
    Mesh backWallMesh;
    backWallMesh.loadOFF ("models/room/back.off");
    Material backWallMat (1.0f, 0.0f, Vec3Df (1.0f, 1.0f, 1.0f));
    Object backWall (backWallMesh, backWallMat);
    objects.push_back (backWall);

    /* LeftWall */
    Mesh leftWallMesh;
    leftWallMesh.loadOFF ("models/room/left.off");
    Material leftWallMat (1.0f, 0.0f, Vec3Df (1.0f, 0.0f, 0.0f));
    Object leftWall (leftWallMesh, leftWallMat);
    objects.push_back (leftWall);

    /* RightWall */
    Mesh rightWallMesh;
    rightWallMesh.loadOFF ("models/room/right.off");
    Material rightWallMat (1.0f, 0.0f, Vec3Df (0.0f, 1.0f, 0.0f));
    Object rightWall (rightWallMesh, rightWallMat);
    objects.push_back (rightWall);

   /* FloorWall */
    Mesh floorWallMesh;
    floorWallMesh.loadOFF ("models/room/floor.off");
    Material floorWallMat (1.0f, 0.0f, Vec3Df (1.0f, 0.0f, 1.0f));
    Object floorWall (floorWallMesh, floorWallMat);
    objects.push_back (floorWall);

   /* FrontWall */
    Mesh frontWallMesh;
    frontWallMesh.loadOFF ("models/room/front.off");
    Material frontWallMat (1.0f, 0.0f, Vec3Df (1.0f, 1.0f, 1.0f));
    Object frontWall (frontWallMesh, frontWallMat);
    objects.push_back (frontWall);

   /* TopWall */
    Mesh topWallMesh;
    topWallMesh.loadOFF ("models/room/top.off");
    Material topWallMat (1.0f, 0.0f, Vec3Df (0.0f, 1.0f, 1.0f));
    Object topWall (topWallMesh, topWallMat);
    objects.push_back (topWall);
}


/* Group Scene 1: room with cubes */
void Scene::buildRoomScene () {
    
    /* Draw Room */
    buildRoom ();
    
    Mesh rhinoMesh;
    rhinoMesh.loadOFF ("models/room/rhino.off");
    Material rhinoMat (1.0f, 0.0f, Vec3Df (0.6f, 0.6f, 0.7f));
    Object rhino (rhinoMesh, rhinoMat);
    rhino.setTrans ( Vec3Df ( 0.0f, 0.5f, 0.0f ) );
    objects.push_back (rhino);
    /* BigCube */

    //Mesh bigCubeMesh;
    //bigCubeMesh.loadOFF ("models/room/bigCube.off");
    //Material bigCubeMat (1.0f, 0.0f, Vec3Df (0.0f, 0.0f, 1.0f));
    //Object bigCube (bigCubeMesh, bigCubeMat);    
    //bigCube.setTrans ( Vec3Df ( 0.0f, 2.0f, 0.0f ) );
    //objects.push_back (bigCube);
 
    /* Cube */
    //Mesh cubeMesh;
    //cubeMesh.loadOFF ("models/room/cube.off");
    //Material cubeMat (1.0f, 0.2f, Vec3Df (0.0f, 1.0f, 0.0f));
    //Object cube (cubeMesh, cubeMat);
    //objects.push_back (cube);

    /* Mirror sphere */
    Mesh sphereMesh;
    sphereMesh.loadOFF ("models/room/sphere.off");
    Material sphereMat ( 0.0f, 1.0f, Vec3Df ( 1.0f, 1.0f, 1.0f ) );
    Object sphere1 ( sphereMesh, sphereMat );
    sphere1.setTrans ( Vec3Df (  2.0f,  1.0f,  2.0f ) );    
    objects.push_back ( sphere1 );

    Object sphere2 ( sphereMesh, sphereMat );
    sphere2.setTrans ( Vec3Df (  0.0f,  3.0f,  2.0f ) );
    objects.push_back ( sphere2 );

    Object sphere3 ( sphereMesh, sphereMat );
    sphere3.setTrans ( Vec3Df ( -2.0f,  1.0f,  2.0f ) );
    objects.push_back ( sphere3 );

    lights.push_back ( Light ( Vec3Df ( -1.0f,  4.0f, -1.0f ), Vec3Df ( 1.0f, 1.0f, 1.0f ), 0.5f ) );
    //lights.push_back ( Light ( Vec3Df (  0.0f,  4.5f, -2.0f ), Vec3Df ( 0.0f, 1.0f, 0.0f ), 0.5f ) );
    //lights.push_back ( Light ( Vec3Df (  2.0f,  4.5f, -2.0f ), Vec3Df ( 0.0f, 0.0f, 1.0f ), 0.5f ) );
    //lights.push_back ( Light ( Vec3Df ( -2.0f,  4.5f,  0.0f ), Vec3Df ( 1.0f, 1.0f, 0.0f ), 0.5f ) );
    //lights.push_back ( Light ( Vec3Df (  0.0f,  4.5f,  0.0f ), Vec3Df ( 0.0f, 1.0f, 1.0f ), 0.5f ) );
    //lights.push_back ( Light ( Vec3Df (  2.0f,  4.5f,  0.0f ), Vec3Df ( 1.0f, 0.0f, 1.0f ), 0.5f ) );
    //lights.push_back ( Light ( Vec3Df ( -2.0f,  4.5f,  2.0f ), Vec3Df ( 0.0f, 1.0f, 1.0f ), 0.5f ) );
    //lights.push_back ( Light ( Vec3Df (  0.0f,  4.5f,  2.0f ), Vec3Df ( 1.0f, 1.0f, 1.0f ), 0.5f ) );
    //lights.push_back ( Light ( Vec3Df (  2.0f,  4.5f,  2.0f ), Vec3Df ( 0.0f, 0.0f, 0.0f ), 0.5f ) );

}

/* Group Scene 2: Sphere scene */
void Scene::buildSphereScene () {

   /* Dino */
    Mesh dinoMesh;
    dinoMesh.loadOFF ("models/balls/dino.off");
    Material dinoMat (1.0f, 0.2f, Vec3Df (0.3f, 0.7f, 0.3f));
    Object dino (dinoMesh, dinoMat);    
    dino.setTrans (Vec3Df (0.0f, -8.0f, 0.0f));
    //objects.push_back (dino);

     /* Bird */
    Mesh birdMesh;
    birdMesh.loadOFF ("models/balls/bird.off");
    Material birdMat (1.0f, 0.2f, Vec3Df (1.0f, 1.0f, 0.0f));
    Object bird (birdMesh, birdMat);    
    //objects.push_back (bird);

     /* Spheres  */
    Mesh sphereMesh;
    sphereMesh.loadOFF ("models/balls/sphere.off");
    Material sphereMat;
    Object sphere;    

    for(float z = 0; z < 6; z = z+2){
        for(float y = -2; y < 8; y = y+2){
            for(float x = 0; x < 6; x = x+2){
                sphereMat = Material(1.0f, 1.0f, Vec3Df ((x+1)/8, (y+2)/12, (z+2)/6));
                sphere = Object(sphereMesh, sphereMat);    
                sphere.setTrans (Vec3Df (x, z, y));
                objects.push_back (sphere);
            }
        }
    }


  
    /* Floor */
    Mesh floorMesh;
    floorMesh.loadOFF ("models/balls/floor.off");
    Material floorMat (1.0f, 0.0f, Vec3Df (1.0f, 1.0f, 1.0f));
    Object floor (floorMesh, floorMat);
    objects.push_back (floor);

    Light l (Vec3Df (3.0f, 3.0f, 3.0f), Vec3Df (1.0f, 1.0f, 1.0f), 1.0f);
    lights.push_back (l);

    l = Light  (Vec3Df (3.0f, 1.0f, 1.0f), Vec3Df (1.0f, 1.0f, 1.0f), 1.0f);
    lights.push_back (l);


}

void Scene::buildBMWScene() {
    Mesh car;
    Material carMat;

    car.loadOFF ("models/bmw/bmw.off");

    carMat.setColor( Vec3Df(0.5f, 0.8f, 0.9f) );
    carMat.setDiffuse(0.05f);
    carMat.setSpecular(0.95f);

    Object carObj(car, carMat);
    carObj.setTrans(Vec3Df(0.0f, 0.0f, 0.0f));
    objects.push_back(carObj);
    lights.push_back ( Light (Vec3Df (  2.0f,  2.0f, -2.0f), Vec3Df (1.0f, 1.0f, 1.0f), 0.5f) );


    Mesh groundMesh;
    groundMesh.loadOFF ("models/bmw/ground.off");
    Material groundMat;
    groundMat.setDiffuse(1.0f);
    groundMat.setAmbient(1.0f);
    groundMat.setColor( Vec3Df(0.8f, 0.8f, 0.8f) );
    Object ground (groundMesh, groundMat);
    objects.push_back (ground);


    Mesh wallMesh;
    wallMesh.loadOFF ("models/bmw/wall.off");
    Material wallMat (1.0f, 0.2f, Vec3Df (1.0f, 1.0f, 1.0f));
    Object wall (wallMesh, wallMat);
    objects.push_back (wall);


    Mesh topMesh;
    topMesh.loadOFF ("models/bmw/top.off");
    Material topMat (1.0f, 0.2f, Vec3Df (1.0f, 1.0f, 1.0f));
    Object top (topMesh, topMat);
    top.setTrans( Vec3Df(0.0f, 3.0f, 0.0f) );
    objects.push_back (top);
}

