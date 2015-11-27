// *********************************************************
// Object Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Object.h"
#include "math.h"

#define PI 3.14159265358979323846

using namespace std;

void Object::updateBoundingBox () {
    const vector<Vertex> & V = mesh.getVertices ();
    if (V.empty ())
        bbox = BoundingBox ();
    else {
        bbox = BoundingBox (V[0].getPos ());
        for (unsigned int i = 1; i < V.size (); i++)
            bbox.extendTo (V[i].getPos ());
    }
}

void Object::GetPointCloud (
    std::vector< Surfel* >&  oPointCloud
) const {
    Mesh tesselatedMe;
    getMesh ().Tesselate ( 0.01f, tesselatedMe );

    for (
        std::vector< Triangle >::const_iterator tri = tesselatedMe.getTriangles ().begin ();
        tri != tesselatedMe.getTriangles ().end ();
        tri++
    ) {
        Surfel* newSurfel = new Surfel (
            getMaterial (),
            tesselatedMe.getVertices (),
            *tri,
            getTrans ()
        );
        //std::cout << "Creating Surfels" << std::endl;
        oPointCloud.push_back (
            newSurfel
        );
    }
}

/*! \brief Generates bump map to be applied to object's normals as white noise convolved with
 * 2-dimensional Gaussian kernel (a simple way to get surface rough and grainy).
  \param[in] std    Kernel's standard deviation; be careful: large values cause increasing of computation time.
*/
void Object::generateBumps(float std) {
    //white noise generating
    normalmap noise;
    fBumps.resize(BUMP_MAP_SIZE);
    noise.resize(BUMP_MAP_SIZE);
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distributionTeta(0,2*PI);
    std::uniform_real_distribution<float> distributionPhi(0,PI);

    for (int i = 0; i < BUMP_MAP_SIZE; i++) {
        fBumps[i].resize(BUMP_MAP_SIZE);
        noise[i].resize(BUMP_MAP_SIZE);
        for (int j = 0; j < BUMP_MAP_SIZE; j++)
            noise[i][j] = Vec3Df::polarToCartesian( Vec3Df(1, distributionTeta(generator), distributionPhi(generator)) );
    }

    //filtering (convolution)
#define sqr(x) ((x)*(x))
    int size = 3*std;

    for (int i = 0; i < BUMP_MAP_SIZE; i++)
        for (int j = 0; j < BUMP_MAP_SIZE; j++) {
            Vec3Df sum(0.0f, 0.0f, 0.0f);
            float norm = 0;
            float kernel = 0;
            for (int x = max(0, i-size); x < min(BUMP_MAP_SIZE, i+size); x++)
                for (int y = max(0, j-size); y < min(BUMP_MAP_SIZE, j+size); y++) {
                    kernel = exp( -( sqr(x-i) + sqr(y-j) ) / (2*sqr(std)) );
                    sum += kernel * noise[x][y];
                    norm += kernel;
                }
            fBumps[i][j] = sum / norm;
        }
}
