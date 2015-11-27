#ifndef _KDINTERSECTIONDATA_H_
#define _KDINTERSECTIONDATA_H_

#include "Object.h"
#include "Vec3D.h"
#include "kd/KdData.h"

namespace kd {

    /*!
     *  \brief A class that describes a ray's intersection point with the geometry stored in the KD-Tree.
     *
     *  Contains the KdData descriptor of the triangle with which the intersection occurred, the
     *  surface normal at the intersection point, the intersection point, the distance t from the
     *  ray's origin and the barycentric coordinates of the intersection point inside the triangle.
     */
    class KdIntersectionData {

    private:
        KdData*         m_primitive;    //!< The intersected primitive.
        Vec3Df          m_normal;       //!< The normal at intersection point.
        Vec3Df          m_point;        //!< The intersection point.
        double           m_T;            //!< The distance from ray's origin.
        double           m_U;            //!< Barycentric coordinate U.
        double           m_V;            //!< Barycentric coordinate V.

    public:
        /*!
         *  \brief  Default constructor.
         *
         *  Sets the primitive pointer to O.
         */
        inline KdIntersectionData ()
        {
            m_primitive = (KdData*)0x0;
        }

        /*!
         *  \brief  Creates an intersection descriptor from its data.
         *
         *  \param  iPrimitive  The data descriptor of the primitive with which the intersection occurred.
         *  \param  iNormal     The surface normal at the intersection point.
         *  \param  iPoint      The intersection point.
         *  \param  iT          The distance T from ray's origin.
         *  \param  iU          The U barycentric coordinate of the intersection point.
         *  \param  iV          The V barycentric coordinate of the intersection point.
         */
        inline KdIntersectionData (
            KdData*             iPrimitive,
            const Vec3Df&       iNormal,
            const Vec3Df&       iPoint,
            const float&        iT,
            const float&        iU,
            const float&        iV
        )   :   m_primitive ( iPrimitive ),
                m_normal ( iNormal ),
                m_point ( iPoint ),
                m_T ( iT ), m_U ( iU ), m_V ( iV )
        {}

        /*!
         *  \brief  Accesses the index of the intersected triangle on the object's
         *          mesh representation.
         *
         *  \return A constant reference to the intersected triangle's index.
         */
        inline const unsigned int& GetTriangleIndex () const
        {
            return m_primitive->GetTriangleIndex ();
        }

        /*!
         *  \brief  Accesses the object with which the intersection occurred.
         *
         *  \return A constant pointer to the object with which the intersection occurred.
         */
        inline const Object* GetObject () const
        {
            return m_primitive->GetObject ();
        }

        /*!
         *  \brief  Accesses the barycentric coordinates of the intersection point and
         *          the distance from the ray's origin.
         *
         *  \param  oT  Where to store the distance from the ray's origin.
         *  \param  oU  Where to store barycentric coordinate U.
         *  \param  oV  Where to store barycentric coordinate V.
         */
        inline void GetBarycentricCoordinates (
            float&        oT,
            float&        oU,
            float&        oV
        ) const {
            oT = m_T;
            oU = m_U;
            oV = m_V;
        }

        /*!
         *  \brief  Accesses the normal of the surface at intersection point.
         *
         *  \return A constant reference to the normal of the surface at intersection point.
         */
        inline const Vec3Df& GetIntersectionNormal () const
        {
            return m_normal;
        }

        /*!
         *  \brief  Accesses the intersection point.
         *
         *  \return A constant reference to the intersection point.
         */
        inline const Vec3Df& GetIntersectionPoint () const
        {
            return m_point;
        }

    };

}

#endif // _KDINTERSECTIONDATA_H_
