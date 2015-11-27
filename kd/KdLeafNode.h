#ifndef _KDLEAFNODE_H_
#define _KDLEAFNODE_H_

#include "kd/KdData.h"
#include "kd/KdNode.h"
#include "kd/KdIntersectionData.h"
#include "Ray.h"

namespace kd {

    /*!
     *  \brief  A class representing a leaf node of the KD-Tree, storing all
     *          primitives contained in a given region.
     *
     *  Representation of a leaf node that stores but DOES NOT OWN pointers to
     *  various elements contained in the region defined by the node's
     *  bounding box.
     */
    class KdLeafNode
        :   public KdNode
    {

    private:
        KdDataVector        m_elems;    //!< The vector of all primitives contained in the region.

    public:
        /*!
         *  \brief  Creates a leaf node from the primitives contained in it and the region
         *          it represents.
         *
         *  \param  iPrimitives The primitives contained in the node.
         *  \param  iRegion     The region represented by the node.
         */
        inline KdLeafNode (
            const KdDataVector&     iPrimitives,
            const BoundingBox&      iRegion
        )   :   KdNode ( iRegion ),
                m_elems ( iPrimitives )
        {}

        /*!
         *  \brief  Creates a leaf node from the primitives contained in it and the boundaries
         *          of the region it represents.
         *
         *  \param  iPrimitives The primitives contained in the node.
         *  \param  iMinBb      The lower boundary of the region represented by the node.
         *  \param  iMaxBb      The upper boundary of the region represented by the node.
         */
        inline KdLeafNode (
            const KdDataVector&     iPrimitives,
            const Vec3Df&           iMinBb,
            const Vec3Df&           iMaxBb
        )   :   KdNode ( iMinBb, iMaxBb ),
                m_elems ( iPrimitives )
        {}

        /*!
         *  \brief  The default destructor.
         *
         *  Deletes all primitives from memory.
         */
        inline ~KdLeafNode ()
        {}

        /*!
         *  \brief  Tests if a ray intersects any of the primitives contained in the node.
         *
         *  Iterates over all the data descriptors and test intersection against them.
         *  Places the intersection data in oIntersection.
         *
         *  \param  iRay            The ray to be tested.
         *  \param  oIntersection   Where to place the intersection descriptor.
         *  \param  iNear           The minimum distance an intersection can occur.
         *  \param  iFar            The maximum distance an intersection can occur.
         *  \return true iff there is an intersection.
         */
        bool Intersect (
            const Ray&              iRay,
            KdIntersectionData&     oIntersection,
            const float&            iNear,
            const float&            iFar
        ) const;

        /*!
         *  \brief  Tests if a ray intersects any of the surfels of the primitives contained in the node.
         *
         *  Iterates over all the data descriptors and test intersection against their surfels.
         *  Places the intersection data in oIntersection.
         *
         *  \param  iRay            The ray to be tested.
         *  \param  oIntersection   Where to place the intersection descriptor.
         *  \param  iNear           The minimum distance an intersection can occur.
         *  \param  iFar            The maximum distance an intersection can occur.
         *  \return true iff there is an intersection.
         */
        bool IntersectSurfel (
            const Ray&              iRay,
            KdIntersectionData&     oIntersection,
            const float&            iNear,
            const float&            iFar
        ) const;

    };

}

#endif // _KDLEAFNODE_H_
