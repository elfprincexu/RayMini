// ---------------------------------------------------------
// Mesh Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2008 Tamy Boubekeur.
// All rights reserved.
// ---------------------------------------------------------

#include "Mesh.h"
#include <queue>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>

using namespace std;

void Mesh::clear () {
    clearTopology ();
    clearGeometry ();
}

void Mesh::clearGeometry () {
    vertices.clear ();
}

void Mesh::clearTopology () {
    triangles.clear ();
}

void Mesh::unmarkAllVertices () {
    for (unsigned int i = 0; i < vertices.size (); i++)
        vertices[i].unmark ();
}

void Mesh::computeTriangleNormals (vector<Vec3Df> & triangleNormals) {
    for (vector<Triangle>::const_iterator it = triangles.begin ();
         it != triangles.end ();
         it++) {
        Vec3Df e01 (vertices[it->getVertex (1)].getPos () - vertices[it->getVertex (0)].getPos ());
        Vec3Df e02 (vertices[it->getVertex (2)].getPos () - vertices[it->getVertex (0)].getPos ());
        Vec3Df n (Vec3Df::crossProduct (e01, e02));
        n.normalize ();
        triangleNormals.push_back (n);
    }
}

void Mesh::recomputeSmoothVertexNormals (unsigned int normWeight) {
    vector<Vec3Df> triangleNormals;
    computeTriangleNormals (triangleNormals);
    for (std::vector<Vertex>::iterator it = vertices.begin (); it != vertices.end (); it++)
        it->setNormal (Vec3Df (0.0, 0.0, 0.0));
    vector<Vec3Df>::const_iterator itNormal = triangleNormals.begin ();
    vector<Triangle>::const_iterator it = triangles.begin ();
    for ( ; it != triangles.end (); it++, itNormal++) 
        for (unsigned int  j = 0; j < 3; j++) {
            Vertex & vj = vertices[it->getVertex (j)];
            float w = 1.0; // uniform weights
            Vec3Df e0 = vertices[it->getVertex ((j+1)%3)].getPos () - vj.getPos ();
            Vec3Df e1 = vertices[it->getVertex ((j+2)%3)].getPos () - vj.getPos ();
            if (normWeight == 1) { // area weight
                w = Vec3Df::crossProduct (e0, e1).getLength () / 2.0;
            } else if (normWeight == 2) { // angle weight
                e0.normalize ();
                e1.normalize ();
                w = (2.0 - (Vec3Df::dotProduct (e0, e1) + 1.0)) / 2.0;
            } 
            if (w <= 0.0)
                continue;
            vj.setNormal (vj.getNormal () + (*itNormal) * w);
        }
    Vertex::normalizeNormals (vertices);
}

void Mesh::collectOneRing (vector<vector<unsigned int> > & oneRing) const {
    oneRing.resize (vertices.size ());
    for (unsigned int i = 0; i < triangles.size (); i++) {
        const Triangle & ti = triangles[i];
        for (unsigned int j = 0; j < 3; j++) {
            unsigned int vj = ti.getVertex (j);
            for (unsigned int k = 1; k < 3; k++) {
                unsigned int vk = ti.getVertex ((j+k)%3);
                if (find (oneRing[vj].begin (), oneRing[vj].end (), vk) == oneRing[vj].end ())
                    oneRing[vj].push_back (vk);
            }
        }
    }
}

void Mesh::collectOrderedOneRing (vector<vector<unsigned int> > & oneRing) const {
    oneRing.resize (vertices.size ());
    for (unsigned int t = 0; t < triangles.size (); t++) {
        const Triangle & ti = triangles[t];
        for (unsigned int i = 0; i < 3; i++) {
            unsigned int vi = ti.getVertex (i);
            unsigned int vj = ti.getVertex ((i+1)%3);
            unsigned int vk = ti.getVertex ((i+2)%3);
            vector<unsigned int> & oneRingVi = oneRing[vi];
            vector<unsigned int>::iterator begin = oneRingVi.begin ();
            vector<unsigned int>::iterator end = oneRingVi.end ();
            vector<unsigned int>::iterator nj = find (begin, end, vj);
            vector<unsigned int>::iterator nk = find (begin, end, vk);
            if (nj != end && nk == end) {
                if (nj == begin)
                    nj = end;
                nj--;
                oneRingVi.insert (nj, vk);
            } else if (nj == end && nk != end) 
                oneRingVi.insert (nk, vj);
            else if (nj == end && nk == end) {
                oneRingVi.push_back (vk);
                oneRingVi.push_back (vj);
            }
        }
    }
}

void Mesh::computeDualEdgeMap (EdgeMapIndex & dualVMap1, EdgeMapIndex & dualVMap2) {
    for (vector<Triangle>::iterator it = triangles.begin ();
         it != triangles.end (); it++) {
        for (unsigned int i = 0; i < 3; i++) {
            Edge eij (it->getVertex (i), it->getVertex ((i+1)%3)); 
            if (dualVMap1.find (eij) == dualVMap1.end ())
                dualVMap1[eij] = it->getVertex ((i+2)%3);
            else
                dualVMap2[eij] = it->getVertex ((i+2)%3);
        }
    } 
}

void Mesh::markBorderEdges (EdgeMapIndex & edgeMap) {
    for (vector<Triangle>::iterator it = triangles.begin ();
         it != triangles.end (); it++) {
        for (unsigned int i = 0; i < 3; i++) {
            unsigned int j = (i+1)%3;
            Edge eij (it->getVertex (i), it->getVertex (j)); 
            if (edgeMap.find (eij) == edgeMap.end ())
                edgeMap[eij] = 0;
            else 
                edgeMap[eij] += 1;
        }
    } 
}

inline void glVertexVec3Df (const Vec3Df & v) {
    glVertex3f (v[0], v[1], v[2]);
}

inline void glNormalVec3Df (const Vec3Df & n) {
    glNormal3f (n[0], n[1], n[2]);
}
 
inline void glDrawPoint (const Vec3Df & pos, const Vec3Df & normal) {
    glNormalVec3Df (normal);
    glVertexVec3Df (pos);
}

inline void glDrawPoint (const Vertex & v) { 
    glDrawPoint (v.getPos (), v.getNormal ()); 
}

void Mesh::renderGL (bool flat) const {
    glBegin (GL_TRIANGLES);
    for (unsigned int i = 0; i < triangles.size (); i++) {
        const Triangle & t = triangles[i];
        Vertex v[3];
        for (unsigned int j = 0; j < 3; j++)
            v[j] = vertices[t.getVertex(j)];
        if (flat) {
            Vec3Df normal = Vec3Df::crossProduct (v[1].getPos () - v[0].getPos (),
                                                  v[2].getPos () - v[0].getPos ());
            normal.normalize ();
            glNormalVec3Df (normal);
        }
        for (unsigned int j = 0; j < 3; j++) 
            if (!flat)
                glDrawPoint (v[j]);
            else
                glVertexVec3Df (v[j].getPos ());
    }
    glEnd ();
}

void Mesh::loadOFF (const std::string & filename) {
    clear ();
    ifstream input (filename.c_str ());
    if (!input)
        throw Exception ("Failing opening the file.");
    string magic_word;
    input >> magic_word;
    if (magic_word != "OFF")
        throw Exception ("Not an OFF file.");
    unsigned int numOfVertices, numOfTriangles, numOfWhat;
    input >> numOfVertices >> numOfTriangles >> numOfWhat;
    for (unsigned int i = 0; i < numOfVertices; i++) {
        Vec3Df pos;
        Vec3Df col;
        input >> pos;
        vertices.push_back (Vertex (pos, Vec3Df (1.0, 0.0, 0.0)));
    }
    for (unsigned int i = 0; i < numOfTriangles; i++) {
        unsigned int polygonSize;
        input >> polygonSize;
        vector<unsigned int> index (polygonSize);
        for (unsigned int j = 0; j < polygonSize; j++)
            input >> index[j];
        for (unsigned int j = 1; j < (polygonSize - 1); j++)
            triangles.push_back (Triangle (index[0], index[j], index[j+1]));
    }
    input.close ();
    recomputeSmoothVertexNormals (0);
}

void Mesh::Tesselate (
    const float&    iMaxTriangleArea,
    Mesh&           oTesselatedMesh
) const {
    std::vector< Vertex >& outVertices = oTesselatedMesh.getVertices ();
    std::queue< Triangle > triangleQueue;
    
    //std::cout << "Copying mesh vertices...";
    for (
        std::vector< Vertex >::const_iterator vtx = getVertices ().begin ();
        vtx != getVertices ().end ();
        vtx++
    ) {
        outVertices.push_back ( *vtx );
    }
    //std::cout << " OK!" << std::endl;

    //std::cout << "Testing input triangles' areas...";
    for (
        std::vector< Triangle >::const_iterator tri = getTriangles ().begin ();
        tri != getTriangles ().end ();
        tri++
    ) {
        if (
            tri->getArea ( outVertices ) >= iMaxTriangleArea
        ) {
            //std::cout << std::endl << "Triangle " << *tri << " too big. Going in queue.";
            triangleQueue.push ( *tri );
        } else {
            oTesselatedMesh.getTriangles ().push_back ( *tri );
        }
    }
    //std::cout << std::endl << "Enqueue OK!" << std::endl;
    
    //std::cout << "Beginning tesselation: " << std::endl;
    //std::cout << "  - " << triangleQueue.size () << " triangles in queue." <<  std::endl;
    //std::cout << "  - Maximum triangle area: " << iMaxTriangleArea << std::endl;
    //unsigned int firstTriangle = 0;
    //unsigned int lastTriangle = triangleQueue.size ();
    while (
        !triangleQueue.empty ()
        //( lastTriangle - firstTriangle ) > 0
    ) {
        //std::cout << "Popping triangle... Old queue size: " << triangleQueue.size () << ". New queue size: ";
        const Triangle t = triangleQueue.front ();//[firstTriangle++];
        triangleQueue.pop ();
        //std::cout << triangleQueue.size () << std::endl;
        ////std::cout << ( lastTriangle - firstTriangle ) << std::endl;        

        if (
            t.getArea ( outVertices ) >= iMaxTriangleArea
        ) {
            //std::cout << "Triangle " << t << " too big. Splitting...";
            
            const unsigned int& newVtx = outVertices.size ();
            //std::cout << " New vertex index: " << newVtx << std::endl;
            //outVertices.push_back ( t.getBarycenter ( outVertices ) );
            const Vec3Df&   v0 = outVertices[t.getVertex(0)].getPos ();
            const Vec3Df&   v1 = outVertices[t.getVertex(1)].getPos ();
            const Vec3Df&   v2 = outVertices[t.getVertex(2)].getPos ();
            const Vec3Df&   n0 = outVertices[t.getVertex(0)].getNormal ();
            const Vec3Df&   n1 = outVertices[t.getVertex(1)].getNormal ();
            const Vec3Df&   n2 = outVertices[t.getVertex(2)].getNormal ();
            
            Vec3Df e0 = v2 - v1;
            Vec3Df e1 = v2 - v0;
            Vec3Df e2 = v1 - v0;
            const float l0 = e0.normalize (); 
            const float l1 = e1.normalize (); 
            const float l2 = e2.normalize (); 
            
            Vertex newVertex;
            if ( l0 >= l1 && l0 >= l2 ) {
                newVertex = Vertex (
                    v1 + 0.5 * l0 * e0,
                    (n1 + n2) * 0.5f
                );
                triangleQueue.push ( Triangle ( t.getVertex ( 0 ), t.getVertex ( 1 ), newVtx ) ); 
                //lastTriangle++;
                triangleQueue.push ( Triangle ( t.getVertex ( 0 ), newVtx, t.getVertex ( 2 ) ) ); 
                //lastTriangle++;
            } else if ( l1 >= l2 && l1 >= l0 ) {
                newVertex = Vertex (
                    v0 + 0.5 * l1 * e1,
                    (n0 + n2) * 0.5f
                );
                triangleQueue.push ( Triangle ( t.getVertex ( 0 ), t.getVertex ( 1 ), newVtx ) ); 
                //lastTriangle++;
                triangleQueue.push ( Triangle ( t.getVertex ( 1 ), t.getVertex ( 2 ), newVtx ) ); 
                //lastTriangle++;
            } else if ( l2 >= l0 && l2 >= l1 ) {
                newVertex = Vertex (
                    v0 + 0.5 * l2 * e2,
                    (n0 + n1) * 0.5f
                );
                triangleQueue.push ( Triangle ( t.getVertex ( 0 ), newVtx, t.getVertex ( 2 ) ) ); 
                //lastTriangle++;
                triangleQueue.push ( Triangle ( newVtx, t.getVertex ( 1 ), t.getVertex ( 2 ) ) ); 
                //lastTriangle++;
            }
            
            outVertices.push_back ( newVertex );
            
            //std::cout << "Splitting OK! New queue size: " << triangleQueue.size () << std::endl;//( lastTriangle - firstTriangle ) << std::endl;
        } else {
            oTesselatedMesh.getTriangles ().push_back ( t );
        }
    }
    //std::cout << "Tesselation done. Output mesh contains " << oTesselatedMesh.getTriangles ().size () << " triangles." <<std::endl;
}

