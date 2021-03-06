// *********************************************************
// Triangle Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2008 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <iostream>
#include <vector>
#include "Vertex.h"
#include "Vec3D.h"

class Triangle {
public:
    inline Triangle () { init (0, 0, 0); }
    inline Triangle (unsigned int v0, unsigned int v1, unsigned int v2) { init (v0, v1, v2); }
    inline Triangle (const unsigned int * vp) { init (vp[0], vp[1], vp[2]); }
    inline Triangle (const Triangle & it) { init (it.v[0], it.v[1], it.v[2]); }
    inline virtual ~Triangle () {}
    inline Triangle & operator= (const Triangle & it) { init (it.v[0], it.v[1], it.v[2]); return (*this); }
    inline bool operator== (const Triangle & t) const { return (v[0] == t.v[0] && v[1] == t.v[1] && v[2] == t.v[2]); }
    inline unsigned int getVertex (unsigned int i) const { return v[i]; }
    inline void setVertex (unsigned int i, unsigned int vertex) { v[i] = vertex; }
    inline bool contains (unsigned int vertex) const { return (v[0] == vertex || v[1] == vertex || v[2] == vertex); }
  
    inline float getArea (
        const std::vector< Vertex >&    iVertexList
    ) const {
        Vec3Df  e1 = iVertexList[v[1]].getPos () - iVertexList[v[0]].getPos ();
        Vec3Df  e2 = iVertexList[v[2]].getPos () - iVertexList[v[0]].getPos ();

        return 0.5f * Vec3Df::crossProduct ( e1, e2 ).getLength ();
    }

    inline Vec3Df getBarycenter (
        const std::vector< Vertex >&    iVertexList
    ) const {
        Vec3Df  v0 = iVertexList[v[0]].getPos ();
        Vec3Df  v1 = iVertexList[v[1]].getPos ();
        Vec3Df  v2 = iVertexList[v[2]].getPos ();

        return ( v0 + v1 + v2 ) / 3.0f;
    }

protected:
    inline void init (unsigned int v0, unsigned int v1, unsigned int v2) {
        v[0] = v0; v[1] = v1; v[2] = v2;  
    }
  
private:
    unsigned int v[3];
};

extern std::ostream & operator<< (std::ostream & output, const Triangle & t);


#endif // TRIANGLE_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
