#ifndef KDPLANE_H
#define KDPLANE_H

#include "Vec3D.h"

namespace kd {

    /*!
     *  \brief  Describes a plane along which a node's bounding box
     *          can be cut.
     *
     *  This is some sort of constant enum class that allows easy rotation of
     *  cutting planes. All its possible values are allocated in compile-time,
     *  constant and static.
     */
    class KdPlane {

    public:
        static const KdPlane    X_PLANE;    //!< The plane describing a cut along the X axis.
        static const KdPlane    Y_PLANE;    //!< The plane describing a cut along the Y axis.
        static const KdPlane    Z_PLANE;    //!< The plane describing a cut along the Z axis.

    private:
        Vec3Df          m_normal;   //!< The normal of the plane described by the object.

    private:
        /*!
         *  \brief  Creates a plane given it's normal vector.
         *  \param  The normal of the plane.
         */
        inline KdPlane (
            const Vec3Df&           iNormal
        )   :   m_normal ( iNormal )
        {
            m_normal.normalize ();
        }

        /*!
         *  \brief  Private copy constructor to prevent the creation
         *          of copies of KdPlane objects.
         */
        inline KdPlane ( const KdPlane& ) {}

        /*!
         *  \brief  Private assignment operator to prevent the creation
         *          of copies of KdPlane objects.
         *
         *  \return The object itself.
         */
        inline KdPlane& operator=( const KdPlane& ) { return (*this); }

    public:
        /*!
         *  \brief  Accesses the normal of the plane described by the object.
         *
         *  \return A constant reference to the normal of the plane.
         */
        inline const Vec3Df& GetNormal () const
        {
            return m_normal;
        }

        /*!
         *  \brief  Rotates around splitting planes.
         *
         *  A KD-Tree node splitting plane must be perpendicular to its parent
         *  node's splitting plane. This methods allows to rotate around the
         *  splitting planes following the right hand rule (X->Y->Z->X->...).
         *
         *  \return The next plane on the rotation scheme.
         */
        inline const KdPlane& NextPlane () const
        {
            if ( // If X plane, returns Y plane.
                this->GetNormal () == X_PLANE.GetNormal ()
            ) {
                return Y_PLANE;
            } else if ( // If Y plane, returns Z plane.
                this->GetNormal () == Y_PLANE.GetNormal ()
            ) {
                return Z_PLANE;
            } else if ( // If Z plane, returns X plane.
                this->GetNormal () == Z_PLANE.GetNormal ()
            ) {
                return X_PLANE;
            } else { // To avoid compiler warnings.
                return (*this);
            }
        }
    };

}

#endif // KDPLANE_H
