// *********************************************************
// Ray Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Ray.h"
#include <stdio.h>

using namespace std;

static const unsigned int NUMDIM = 3, RIGHT = 0, LEFT = 1, MIDDLE = 2;

bool Ray::intersect (const BoundingBox & bbox, Vec3Df & intersectionPoint) const {
    const Vec3Df & minBb = bbox.getMin ();
    const Vec3Df & maxBb = bbox.getMax ();
    bool inside = true;
    unsigned int  quadrant[NUMDIM];
    register unsigned int i;
    unsigned int whichPlane;
    Vec3Df maxT;
    Vec3Df candidatePlane;
    
    for (i=0; i<NUMDIM; i++)
        if (origin[i] < minBb[i]) {
            quadrant[i] = LEFT;
            candidatePlane[i] = minBb[i];
            inside = false;
        } else if (origin[i] > maxBb[i]) {
            quadrant[i] = RIGHT;
            candidatePlane[i] = maxBb[i];
            inside = false;
        } else	{
            quadrant[i] = MIDDLE;
        }

    if (inside)	{
        intersectionPoint = origin;
        return (true);
    }

    for (i = 0; i < NUMDIM; i++)
        if (quadrant[i] != MIDDLE && direction[i] !=0.)
            maxT[i] = (candidatePlane[i]-origin[i]) / direction[i];
        else
            maxT[i] = -1.;

    whichPlane = 0;
    for (i = 1; i < NUMDIM; i++)
        if (maxT[whichPlane] < maxT[i])
            whichPlane = i;

    if (maxT[whichPlane] < 0.) return (false);
    for (i = 0; i < NUMDIM; i++)
        if (whichPlane != i) {
            intersectionPoint[i] = origin[i] + maxT[whichPlane] *direction[i];
            if (intersectionPoint[i] < minBb[i] || intersectionPoint[i] > maxBb[i])
                return (false);
        } else {
            intersectionPoint[i] = candidatePlane[i];
        }
    return (true);			
}

bool Ray::intersect (
    const Vertex&   v0,
    const Vertex&   v1,
    const Vertex&   v2,
    float&          t,
    float&          u,
    float&          v
) const {
    // Translates v2 and v1 to the origin of the
    // coordinate system.
    Vec3Df E1 = v1.getPos() - v0.getPos();
    Vec3Df E2 = v2.getPos() - v0.getPos();

    // P = direction x E2
    Vec3Df P = Vec3Df::crossProduct(direction,E2);
    // The determinant of [-direction, E1, E2]
    double det = Vec3Df::dotProduct(E1,P);

    // Tests if ray and triangle are parallel, considering an error
    if ( det < EPSILON )
        return (false);

    double invDet = 1 / det;
    // Translates the origin of the ray along with the triangle
    Vec3Df T = origin - v0.getPos();

    u = (Vec3Df::dotProduct(T,P)) * invDet;
    if ( u < 0.0f || u > 1.0f )
        return (false);

    // Q = T x E1
    Vec3Df Q = Vec3Df::crossProduct(T,E1);
    v = Vec3Df::dotProduct(direction,Q) * invDet;

    if ( v < 0.0f || u + v > 1.0f )
        return (false);

    // at this stage we can compute t to find out where
    // the intersection point is on the line
    t = Vec3Df::dotProduct(E2,Q) * invDet;

    return true;
}

// Ray-surfel intersection test.
bool Ray::intersect (
    const Surfel&   iSurfel,
    Vec3Df&         oPosition
) const {
    Vec3Df originDist = iSurfel.GetPosition () - origin;
    double t = Vec3Df::dotProduct ( originDist, iSurfel.GetNormal () )
            / Vec3Df::dotProduct (  direction, iSurfel.GetNormal () );

    oPosition = origin + t * direction;

    if (
        iSurfel.Contains ( oPosition )
    ) {
        return true;
    } else {
        return false;
    }
}
