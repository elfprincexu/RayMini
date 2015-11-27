#ifndef _KDDATA_H_
#define _KDDATA_H_

#include "Object.h"
#include "Vertex.h"
#include "Surfel.h"

#undef GetObject       //special for windows

namespace kd {

    /*!
     *  \brief A class that contains data that will be stored in tree's leaf nodes.
     *
     *  Contains the object that owns the triangle contained in the node, the index
     *  reference of the trianle, a surfel representing the triangle and all three
     *  vertices with their positions translated.
     */
    class KdData {

    private:
        const Object*   m_object;           //!< The triangle owner.
        unsigned int    m_triangleIndex;    //!< The triangle's index on owner's lists.

        Vertex          m_vertices[3];      //!< The translated vertices.
        Surfel          m_surfel;           //!< The surfel representation of the triangle.

    public:
        /*!
         *  \brief  Constructs a new KdData object.
         *  \param  iTriangleIndex  The index of the triangle to be stored.
         *  \param  iObject         The owner of the triangle.
         */
        inline KdData (
            const unsigned int&     iTriangleIndex,
            const Object*           iObject
        )   :   m_triangleIndex ( iTriangleIndex )
        {
            // Copies the object's pointer.
            m_object = iObject;

            // The mesh representation of the object.
            const Mesh& mesh = m_object->getMesh();

            // The indices of the vertices on the mesh.
            unsigned int vid0 = mesh.getTriangles ()[m_triangleIndex].getVertex ( 0 );
            unsigned int vid1 = mesh.getTriangles ()[m_triangleIndex].getVertex ( 1 );
            unsigned int vid2 = mesh.getTriangles ()[m_triangleIndex].getVertex ( 2 );

            // Copies the vertices to our object.
            m_vertices[0] = mesh.getVertices ( )[vid0];
            m_vertices[1] = mesh.getVertices ( )[vid1];
            m_vertices[2] = mesh.getVertices ( )[vid2];

            // Translates the vertices.
            m_vertices[0].setPos ( m_vertices[0].getPos () + m_object->getTrans () );
            m_vertices[1].setPos ( m_vertices[1].getPos () + m_object->getTrans () );
            m_vertices[2].setPos ( m_vertices[2].getPos () + m_object->getTrans () );

            // Creates the surfel representation of the triangle.
            m_surfel = Surfel (
                m_object->getMaterial (),
                m_vertices[0],
                m_vertices[1],
                m_vertices[2]
            );
        }

        /*!
         *  \brief  Default destructor.
         */
        ~KdData () {}

        /*!
         *  \brief  Tests if this primitive intersects a given
         *          bounding box.
         *
         *  An intersection occurs when there's at least two vertices on opposing
         *  sides of the bounding box.
         *
         *  \param  iRegion         The bounding box to test against.
         *  \param  oIsContained    Where to signal whether or not the primitive is contained in the region.
         *  \return true iff there is an intersection.
         */
        inline bool Intersects (
            const BoundingBox&  iRegion,
            bool&               oIsContained
        ) const {
            // If all three vertices are contained inside the
            // bounding box, there is an intersection and
            // the primitive is contained in it.
            if (
                    iRegion.contains( m_vertices[0].getPos() )
                &&  iRegion.contains( m_vertices[1].getPos() )
                &&  iRegion.contains( m_vertices[2].getPos() )
            ) {
                oIsContained = true;
                return true;
            } else {
                oIsContained = false;
            }

            // If all of the vertices X, Y or Z coordinates are below their
            // corresponding coordinate on the lower boundary of the
            // bounding box, there's no intersection.
            for ( unsigned int i = 0; i < 3; i++ ) {
                if (
                    m_vertices[0].getPos()[i] < iRegion.getMin()[i] &&
                    m_vertices[1].getPos()[i] < iRegion.getMin()[i] &&
                    m_vertices[2].getPos()[i] < iRegion.getMin()[i]
                ) {
                    return false;
                }
            }

            // If all of the vertices X, Y or Z coordinates are above their
            // corresponding coordinate on the lower boundary of the
            // bounding box, there's no intersection.
            for ( unsigned int i = 0; i < 3; i++ ) {
                if (
                    m_vertices[0].getPos()[i] > iRegion.getMax()[i] &&
                    m_vertices[1].getPos()[i] > iRegion.getMax()[i] &&
                    m_vertices[2].getPos()[i] > iRegion.getMax()[i]
                ) {
                    return false;
                }
            }

            // Otherwise, an intersection occurs.
            return true;
        }

        /*!
         *  \brief  Accesses the surface element representation stored in the node.
         *
         *  \return A constant reference to the surface element stored in the node.
         */
        inline const Surfel& GetSurfel () const
        {
            return m_surfel;
        }

        /*!
         *  \brief  Accesses the surface element representation stored in the node.
         *
         *  \return A reference to the surface element stored in the node.
         */
        inline Surfel& GetSurfel ()
        {
            return m_surfel;
        }


        /*!
         *  \brief  Accesses the object that owns the triangle stored in the node.
         *
         *  \return A constant pointer to the object that owns the triangle stored in the node.
         */
        inline const Object* GetObject () const
        {
            return m_object;
        }

        /*!
         *  \brief  Calculates the barycenter of the triangle stored in the
         *          node.
         *
         *  \return A 3D vector representing the point in space of the triangle's barycenter.
         */
        inline Vec3Df GetBarycenter () const
        {
            return ( m_vertices[0].getPos()
                        + m_vertices[1].getPos()
                        + m_vertices[2].getPos() ) / 3.0f;
        }


        /*!
         *  \brief  Accesses the index of the triangle contained in the node,
         *          related to the
         *
         *  \return A constant reference to the surface element stored in the node.
         */
        inline const unsigned int& GetTriangleIndex () const
        {
            return m_triangleIndex;
        }

        /*!
         *  \brief  Accessor operator for the vertices stored in the node.
         *
         *  \param  iIndex  The index of the vertex.
         *  \return A constant reference to the corresponding vertex.
         */
        inline const Vertex& operator[] (
            const int& iIndex
        ) const {
            return m_vertices[iIndex];
        }


        /*!
         *  \brief  Accessor operator for the vertices stored in the node.
         *
         *  \param  iIndex  The index of the vertex.
         *  \return A reference to the corresponding vertex.
         */
        inline Vertex& operator[] (
            const int& iIndex
        ) {
            return m_vertices[iIndex];
        }

    };

    /*!
     *  \brief  The vector to be used to store KdData elements.
     */
    typedef std::vector< KdData* >  KdDataVector;

}

#endif // _KDDATA_H_
