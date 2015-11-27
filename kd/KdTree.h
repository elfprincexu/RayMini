#ifndef _KDTREE_H_
#define _KDTREE_H_

#include <limits>
#include "BoundingBox.h"
#include "Ray.h"
#include "Object.h"
#include "Vec3D.h"
#include "Surfel.h"
#include "MathUtils.h"
#include "kd/KdNode.h"
#include "kd/KdMiddleNode.h"

// TODO
//  - Make KdData and KdIntersectionData interfaces.
//  - Implement SAH
namespace kd {

    /*!
     *  \brief  An implementation of a KD-Tree to speed up ray
     *  intersection checks.
     *
     *  A KD-Tree descriptor that stores and OWNS pointers to all primitive
     *  data descriptors contained in its leaf nodes.
     */
    class KdTree {
    private:
        KdNode*         m_root;     //!< The root node of the tree.
        KdDataVector    m_elems;    //!< Keeps copies of data descriptor pointers for memory cleanup.
        unsigned int    m_depth;    //!< The maximum depth of the tree.

    public:
        /*!
         *  \brief  Creates a KD-Tree from the primitive descriptors it should
         *          contain.
         *
         *  Calculates the bounding box that best fits all vertices inside the region, allocates
         *  a root node with all data and splits it.
         *
         *  \param  iPrimitives The list of primitives that should be indexed by the KD-Tree.
         */
        inline KdTree (
            const KdDataVector&     iPrimitives
        )   :   m_elems ( iPrimitives )
        {
            Vec3Df minBb;
            Vec3Df maxBb;

            // Calculate lowed and upper boundaries of the bounding box
            // that best surrounds all data in the KD-Tree.
            GetMinMax (
                iPrimitives,
                minBb,
                maxBb
            );

            BoundingBox rootRegion ( minBb, maxBb );

            KdMiddleNode* rootNode = new KdMiddleNode ( rootRegion );
            m_depth = rootNode->Split (
                0u,
                KdPlane::X_PLANE,
                iPrimitives
            );

            //KdLeafNode* rootNode = new KdLeafNode ( iPrimitives, rootRegion );
            //m_depth = 0;

            m_root = rootNode;
        }

        /*!
         *  \brief  Creates a KD-Tree from the primitive descriptors it should
         *          contain and a bounding box that surrounds all of them.
         *
         *  \param  iRegion     A region that surrounds all data on the KD-Tree.
         *  \param  iPrimitives The list of primitives that should be indexed by the KD-Tree.
         */
        inline KdTree (
            const BoundingBox&      iRegion,
            const KdDataVector&     iPrimitives
        )   :   m_elems ( iPrimitives )
        {
            // Allocates root node from input region.
            KdMiddleNode* rootNode = new KdMiddleNode ( iRegion );
            m_root = rootNode;

            // Splits root node.
            m_depth = rootNode->Split (
                0u,
                KdPlane::X_PLANE,
                iPrimitives
            );
        }

        /*!
         *  \brief  Destroys the KD-Tree instance and all data it contains.
         */
        inline ~KdTree ()
        {
            if ( m_root ) {
                delete m_root;
                m_root = (KdNode*)0x0;
            }

            // Iterates over all primitive descriptors and free
            // the memory allocated for them.
            for (
                unsigned int i = 0;
                i < m_elems.size ();
                i++
            ) {
                KdData* ptr = m_elems[i];

                if ( ptr ) {
                    delete ptr;
                    ptr = (KdData*)0x0;
                }
            }
        }

        /*!
         *  \brief  Tests intersection of a ray against the primitives contained
         *          in the KD-Tree.
         *
         *  Checks for intersection against the root's bounding box. If there's an
         *  intersection, propagate the test from the root.
         *
         *  \param  iRay            The ray to be tested.
         *  \param  oIntersection   Where to place the intersection descriptor.
         *  \param  iNear           The minimum distance an intersection can occur (defaults to 0).
         *  \param  iFar            The maximum distance an intersection can occur (defaults to infinity).
         *  \return true iff there is an intersection.
         */
        inline bool Intersect (
            const Ray&              iRay,
            KdIntersectionData&     oIntersection,
            const float&            iNear=-1.0f,
            const float&            iFar=-1.0f
        ) const {
            Vec3Df intersectionPoint;

            // Check for default near and far plane distances.
            const double nearPlane = ( iNear < 0.0f ) ? EPSILON : iNear;
            const double farPlane  = ( iFar  < 0.0f ) ? std::numeric_limits<double>::infinity () : iFar;

            // If the ray intersects the root's bounding box,
            // propagate the intersection test.
            if (
                iRay.intersect (
                    m_root->GetRegion (),
                    intersectionPoint
                )
            ) {
                //Ray newRay ( intersectionPoint, iRay.getDirection () );
                Ray newRay ( iRay );

                return m_root->Intersect (
                    newRay,
                    oIntersection,
                    nearPlane,
                    farPlane
                );
            }

            // Else, there can't be an intersection with the geometry.
            return false;
        }

        /*!
         *  \brief  Tests intersection of a ray against the surfels contained
         *          in the KD-Tree.
         *
         *  Checks for intersection against the root's bounding box. If there's an
         *  intersection, propagate the test from the root.
         *
         *  \param  iRay            The ray to be tested.
         *  \param  oIntersection   Where to place the intersection descriptor.
         *  \param  iNear           The minimum distance an intersection can occur (defaults to 0).
         *  \param  iFar            The maximum distance an intersection can occur (defaults to infinity).
         *  \return true iff there is an intersection.
         */
        inline bool IntersectSurfel (
            const Ray&              iRay,
            KdIntersectionData&     oIntersection,
            const float&            iNear=-1.0f,
            const float&            iFar=-1.0f
        ) const {
            Vec3Df intersectionPoint;

            // Check for default near and far plane distances.
            const double nearPlane = ( iNear < 0.0f ) ? 0.0f : iNear;
            const double farPlane  = ( iFar  < 0.0f ) ? std::numeric_limits<double>::infinity () : iFar;

            // If the ray intersects the root's bounding box,
            // propagate the intersection test.
            if (
                iRay.intersect (
                    m_root->GetRegion (),
                    intersectionPoint
                )
            ) {
                //Ray newRay ( intersectionPoint, iRay.getDirection () );
                Ray newRay ( iRay );

                return m_root->IntersectSurfel (
                    newRay,
                    oIntersection,
                    nearPlane,
                    farPlane
                );
            }

            // Else, there can't be an intersection with any of the surfels.
            return false;
        }

    };
}

#endif // _KDTREE_H_
