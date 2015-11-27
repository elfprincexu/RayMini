#ifndef _KDNODE_H_
#define _KDNODE_H_

#include "Ray.h"
#include "BoundingBox.h"
#include "kd/KdData.h"
#include "kd/KdIntersectionData.h"
#include "MathUtils.h"

namespace kd {

    const unsigned int MaxElems = 1;    //!< The maximum number of elements contained in a leaf node.
    const unsigned int MaxDepth = 20;   //!< The maximum depth a node can be located with respect to the root.

    /*!
     *  \brief  Finds the maximum and minimum vertex positions inside a
     *          primitive vector.
     *
     *  \param  iPrimitives The vector to be checked.
     *  \param  oMin        Where to place the minimum value.
     *  \param  oMax        Where to place the maximum value.
     */
    inline void GetMinMax (
        const KdDataVector&         iPrimitives,
        Vec3Df&                     oMin,
        Vec3Df&                     oMax
    ) {
        double inf = std::numeric_limits<double>::infinity ();

        double xMin = inf, xMax = -inf;
        double yMin = inf, yMax = -inf;
        double zMin = inf, zMax = -inf;
        for (
            KdDataVector::const_iterator it = iPrimitives.begin ();
            it != iPrimitives.end ();
            it++
        ) {
            const Vec3Df& v0 = ( (*it)->operator[] ( 0 ) ).getPos ();
            const Vec3Df& v1 = ( (*it)->operator[] ( 1 ) ).getPos ();
            const Vec3Df& v2 = ( (*it)->operator[] ( 2 ) ).getPos ();

            xMin = min ( min ( min ( xMin, v0[0] ), v1[0] ), v2[0] );
            yMin = min ( min ( min ( yMin, v0[1] ), v1[1] ), v2[1] );
            zMin = min ( min ( min ( zMin, v0[2] ), v1[2] ), v2[2] );

            xMax = max ( max ( max ( xMax, v0[0] ), v1[0] ), v2[0] );
            yMax = max ( max ( max ( yMax, v0[1] ), v1[1] ), v2[1] );
            zMax = max ( max ( max ( zMax, v0[2] ), v1[2] ), v2[2] );
        }
        oMin = Vec3Df ( xMin, yMin, zMin );
        oMax = Vec3Df ( xMax, yMax, zMax );
    }

    /*!
     *  \brief  The base class of any KD-Tree node.
     */
    class KdNode {

    private:
        BoundingBox             m_region;   //!< The region in space represented by the node.

    protected:
        /*!
         *  \brief  Creates an empty node.
         */
        inline KdNode ()
            :   m_region ()
        {}

        /*!
         *  \brief  Creates a node from it's region's boundaries.
         *
         *  \param  iMinBb      The region's lower boundary.
         *  \param  iMaxBb      The region's upper boundary.
         */
        inline KdNode (
            const Vec3Df&           iMinBb,
            const Vec3Df&           iMaxBb
        )   :   m_region ( iMinBb, iMaxBb )
        {}

        /*!
         *  \brief  Creates a node from it's region.
         *
         *  \param  iRegion     The region's represented by the node.
         */
        inline KdNode (
            const BoundingBox&      iRegion
        )   :   m_region ( iRegion )
        {}

    public:
        /*!
         *  \brief  Destructs the node.
         */
        virtual ~KdNode ()
        {}

        /*!
         *  \brief  Accesses the bounding box describing the region
         *          represented by the node.
         *  \return A constant reference to the bounding box object describing the node's region.
         */
        inline const BoundingBox& GetRegion() const
        {
            return m_region;
        }

        /*!
         *  \brief  Accesses the bounding box describing the region
         *          represented by the node.
         *  \return A reference to the bounding box object describing the node's region.
         */
        inline BoundingBox& GetRegion() {
            return m_region;
        }

        /*!
         *  \brief  Tests intersection of a ray against the primitives contained
         *          in the node.
         *
         *  Check specific nodes' documentations for intersection mechanics.
         *
         *  \param  iRay            The ray to be tested.
         *  \param  oIntersection   Where to place the intersection descriptor.
         *  \param  iNear           The minimum distance an intersection can occur.
         *  \param  iFar            The maximum distance an intersection can occur.
         *  \return true iff there is an intersection.
         */
        virtual bool Intersect (
            const Ray&              iRay,
            KdIntersectionData&     oIntersection,
            const float&            iNear,
            const float&            iFar
        ) const = 0;

        /*!
         *  \brief  Tests intersection of a ray against the surfels contained
         *          in the node.
         *
         *  Check specific nodes' documentations for intersection mechanics.
         *
         *  \param  iRay            The ray to be tested.
         *  \param  oIntersection   Where to place the intersection descriptor.
         *  \param  iNear           The minimum distance an intersection can occur.
         *  \param  iFar            The maximum distance an intersection can occur.
         *  \return true iff there is an intersection.
         */
        virtual bool IntersectSurfel (
            const Ray&              iRay,
            KdIntersectionData&     oIntersection,
            const float&            iNear,
            const float&            iFar
        ) const = 0;

    };

}

#endif // _KDNODE_H_
