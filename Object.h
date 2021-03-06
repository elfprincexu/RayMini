// *********************************************************
// Object Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>
#include <vector>

#include "Mesh.h"
#include "Material.h"
#include "BoundingBox.h"
#include "Surfel.h"

class Object {
public:
    inline Object () : fBumpsLevel(0.0f) {}
    inline Object (const Mesh & mesh, const Material & mat) : mesh (mesh), mat (mat), fBumpsLevel(0.0f) {
        updateBoundingBox ();
        generateBumps();
    }
    virtual ~Object () {}

    inline const Vec3Df & getTrans () const { return trans;}
    inline void setTrans (const Vec3Df & t) { trans = t; }

    inline const Mesh & getMesh () const { return mesh; }
    inline Mesh & getMesh () { return mesh; }
    
    inline const Material & getMaterial () const { return mat; }
    inline Material & getMaterial () { return mat; }

    inline const BoundingBox & getBoundingBox () const { return bbox; }
    void updateBoundingBox ();

    void GetPointCloud (
        std::vector< Surfel* >&  oPointCloud
    ) const;

    void generateBumps(float std = 1.0f);

    inline float getBumpLevel() const       { return fBumpsLevel; }  //!< Returns bump amplitude.
    inline void setBumpLevel(float val)     { fBumpsLevel = val; }   //!< Sets bump amplitude (0 = no bumps).

    /*! \brief Computes normal at requested point, affected by bump map application.
      \param[in] trgIndex    Number of triangle in object's mesh.
      \param[in] u           Barycentric coordinate of the point in which the normal is requested.
      \param[in] v           Barycentric coordinate of the point in which the normal is requested.
      \return normal in point (u,v) of triangle trgIndex.
    */
    inline Vec3Df getBumpedNormal(unsigned int trgIndex, float u, float v) const {
        Triangle trg = mesh.getTriangles()[trgIndex];
        Vertex
            v0 = mesh.getVertices()[ trg.getVertex(0) ],
            v1 = mesh.getVertices()[ trg.getVertex(1) ],
            v2 = mesh.getVertices()[ trg.getVertex(2) ];
        Vec3Df normal = ( 1 - u - v ) * v0.getNormal ()
                      +       u       * v1.getNormal ()
                      +       v       * v2.getNormal ();
        normal.normalize ();
        int
            x = u * Vec3Df::distance(v1.getPos(), v0.getPos()) * BUMP_MAP_SCALE,
            y = v * Vec3Df::distance(v2.getPos(), v0.getPos()) * BUMP_MAP_SCALE;
        normal += fBumps[x % BUMP_MAP_SIZE][y % BUMP_MAP_SIZE] * fBumpsLevel;
        normal.normalize();
        return normal;
    }

private:
    Mesh mesh;
    Material mat;
    BoundingBox bbox;
    Vec3Df trans;

    static const int
        BUMP_MAP_SIZE = 100,        //!< Size of bump map
        BUMP_MAP_SCALE = 1000;      //!< Bump map scale factor
    typedef std::vector<std::vector<Vec3Df>> normalmap;     //!< Just a vector field.
    normalmap fBumps;               //!< Bump map (generated by \a generateBumps(...) )
    float fBumpsLevel;              //!< Bump amplitude
};


#endif // Scene_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
