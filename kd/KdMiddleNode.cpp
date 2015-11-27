#include "kd/KdMiddleNode.h"

using namespace kd;

/*!
 *  \brief  Updates a node based on it's primitives list.
 *
 *  If the child node's list is empty, does not allocate memory
 *  for any node and sets the pointer to 0. If either the maximum depth
 *  has been reached or the list contains less than the maximum number
 *  of elements allowed in a leaf, create a leaf node. Else, create
 *  an intermediary node and split it.
 *
 *  \param  iPrimitives The primitives that should be placed on the child node.
 *  \param  iChildBb    The bounding box of the child node.
 *  \param  iNextDepth  The depth of the child node.
 *  \param  iSplitPlane The plane defining the direction the bounding box was cut.
 *  \param  oChildDepth Where to place the new child's depth.
 *  \param  oChildNode  Where to store the new child node.
 */
inline void UpdateChildNode (
    const KdDataVector&     iPrimitives,
    const BoundingBox&      iChildBb,
    const unsigned int&     iNextDepth,
    const KdPlane&          iSplitPlane,
    unsigned int&           oChildDepth,
    KdNode*&                oChildNode
) {
    // If no primitives should be placed inside the child node.
    if (
        iPrimitives.size () == 0
    ) {
        // No node is allocated.
        oChildNode = (KdNode*)0x0;
    } else if (
            ( iPrimitives.size () <= MaxElems )
        ||  ( iNextDepth >= ( MaxDepth - 1 ) )
    ) {
        // If we have reached maximum depth or the
        // list of primitives contains less than the maximum number
        // allowed per node, allocate a new leaf node and place
        // primitives inside it.
        oChildNode = new KdLeafNode (
            iPrimitives,
            iChildBb
        );

        // Leaf nodes' trees have null depths.
        oChildDepth = 0u;
    } else {
        // If we can still split the node, create a new intermediary node.
        KdMiddleNode* newChildNode = (KdMiddleNode*)0x0;
        oChildNode = newChildNode = new KdMiddleNode (
            iChildBb
        );

        // Split the node and store it's tree's height.
        oChildDepth = newChildNode->Split (
            iNextDepth,
            iSplitPlane,
            iPrimitives
        );
    }
}

/*!
 * \inheaderfile
 */
int KdMiddleNode::Split (
    const unsigned int&     iDepth,
    const KdPlane&          iSplitPlane,
    const KdDataVector&     iData
) {
    // Child nodes' bounding boxes.
    BoundingBox leftBb, rightBb;

    // Cuts the node's bounding box along a plane that passes
    // by it's center.
    Vec3Df cutPosition = GetRegion ().getCenter ();

    // How far away from the splitting plane the
    // lower boundary of the bounding box is located.
    Vec3Df projMin = Vec3Df::projectOntoVector(
        cutPosition - GetRegion ().getMin (),
        iSplitPlane.GetNormal ()
    );

    // How far away from the splitting plane the
    // upper boundary of the bounding box is located.
    Vec3Df projMax = Vec3Df::projectOntoVector(
        GetRegion ().getMax () - cutPosition,
        iSplitPlane.GetNormal ()
    );

    // The lower bounding box goes from the original lower boundary to
    // the upper boundary displaced by projMax towards the splitting plane.
    leftBb.init ( GetRegion ().getMin () );
    leftBb.extendTo ( GetRegion().getMax () - projMax );

    // The upper bounding box goes from the lower bounding box lower boundary
    // displaced by projMin towards the splitting plane, to the original
    // upper boundary.
    rightBb.init  ( GetRegion ().getMin () + projMin );
    rightBb.extendTo ( GetRegion ().getMax () );

    // The lists of primitives contained in each child node.
    KdDataVector leftPrimitives;
    KdDataVector rightPrimitives;

    // Tests intersection of each primitive against each child node
    // and place them in the corresponding list.
    for (
        KdDataVector::const_iterator it = iData.begin();
        it != iData.end();
        it++
    ) {
        KdData* primitive = *it;

        bool isContained = false;
        if (
            primitive->Intersects ( rightBb, isContained )
        ) {
            rightPrimitives.push_back ( primitive );
        }
        if (
            primitive->Intersects ( leftBb, isContained )
        ) {
            leftPrimitives.push_back ( primitive );
        }
    }

    // Rotates the splitting plane.
    const KdPlane& nextSplitPlane = iSplitPlane.NextPlane ();

    // Child nodes' depths.
    unsigned int nextDepth = iDepth + 1;
    unsigned int rDepth = 0;
    unsigned int lDepth = 0;

    // Updates right node.
    UpdateChildNode (
        rightPrimitives,
        rightBb,
        nextDepth,
        nextSplitPlane,
        rDepth,
        m_rChild
    );

    // Updates left node.
    UpdateChildNode (
        leftPrimitives,
        leftBb,
        nextDepth,
        nextSplitPlane,
        lDepth,
        m_lChild
    );

    // This node's tree has maximum depth equal to the maximum
    // between it's child node's trees' maximum depths plus 1.
    return ( max( rDepth, lDepth ) + 1 );
}

/*!
 * \inheaderfile
 */
bool KdMiddleNode::Intersect (
    const Ray&              iRay,
    KdIntersectionData&     oIntersection,
    const float&            iNear,
    const float&            iFar
) const {
    // The point where the intersection ocurred.
    Vec3Df intersectionPoint;

    // Signals intersections with each child node.
    bool intersectsLeft = false;
    bool intersectsRight = false;

    KdIntersectionData leftData;
    KdIntersectionData rightData;

    // Tests for intersection with right tree.
    if (
            m_rChild
        &&  iRay.intersect ( m_rChild->GetRegion (), intersectionPoint )
    ) {
        //Ray newRay ( intersectionPoint, iRay.getDirection () );
        Ray newRay ( iRay.getOrigin (), iRay.getDirection () );

        intersectsRight = m_rChild->Intersect (
            newRay,
            rightData,
            iNear,
            iFar
        );
    }
    // Tests for intersection with left tree.
    if (
            m_lChild
        &&  iRay.intersect ( m_lChild->GetRegion (), intersectionPoint )
    ) {
        //Ray newRay ( intersectionPoint, iRay.getDirection () );
        Ray newRay ( iRay.getOrigin (), iRay.getDirection () );

        intersectsLeft = m_lChild->Intersect (
            newRay,
            leftData,
            iNear,
            iFar
        );
    }

    if ( // If there's no intersection, return false.
            !intersectsLeft
        &&  !intersectsRight ) {
        return false;
    } else if ( // Intersection only with right tree.
        !intersectsLeft
    ) {
        oIntersection = rightData;
    } else if ( // Intersection only with left tree.
        !intersectsRight
    ) {
        oIntersection = leftData;
    } else if ( // Intersection with both trees. Ignores two intersections on same triangle.
        (
                ( rightData.GetObject () != leftData.GetObject () )
        ) || (
                ( rightData.GetObject () == leftData.GetObject () )
            &&  ( rightData.GetTriangleIndex () != leftData.GetTriangleIndex () )
        )
    ) {
        const Vec3Df&  leftPoint =  leftData.GetIntersectionPoint();
        const Vec3Df& rightPoint = rightData.GetIntersectionPoint();

        double  leftDist = Vec3Df::squaredDistance (  leftPoint, iRay.getOrigin() );
        double rightDist = Vec3Df::squaredDistance ( rightPoint, iRay.getOrigin() );

        if (
            ( rightDist <= leftDist )
        ) {
            oIntersection = rightData;
        } else {
            oIntersection = leftData;
        }
    } else {
        oIntersection = rightData;
    }

    return true;
}

/*!
 * \inheaderfile
 */
bool KdMiddleNode::IntersectSurfel (
    const Ray&              iRay,
    KdIntersectionData&     oIntersection,
    const float&            iNear,
    const float&            iFar
) const {
    Vec3Df intersectionPoint;

    bool intersectsLeft = false;
    bool intersectsRight = false;

    KdIntersectionData leftData;
    KdIntersectionData rightData;

    // Tests for intersection with right tree.
    if (
            m_rChild
        &&  iRay.intersect ( m_rChild->GetRegion (), intersectionPoint )
    ) {
        //Ray newRay ( intersectionPoint, iRay.getDirection () );
        Ray newRay ( iRay.getOrigin (), iRay.getDirection () );

        intersectsRight = m_rChild->IntersectSurfel (
            newRay,
            rightData,
            iNear,
            iFar
        );
    }
    // Tests for intersection with left tree.
    if (
            m_lChild
        &&  iRay.intersect ( m_lChild->GetRegion (), intersectionPoint )
    ) {
        //Ray newRay ( intersectionPoint, iRay.getDirection () );
        Ray newRay ( iRay.getOrigin (), iRay.getDirection () );

        intersectsLeft = m_lChild->IntersectSurfel (
            newRay,
            leftData,
            iNear,
            iFar
        );
    }

    if ( // If there's no intersection, return false.
            !intersectsLeft
        &&  !intersectsRight ) {
        return false;
    } else if ( // Intersection only with right tree.
        !intersectsLeft
    ) {
        oIntersection = rightData;
    } else if ( // Intersection only with left tree.
        !intersectsRight
    ) {
        oIntersection = leftData;
    } else if ( // Intersection with both trees. Ignores two intersections on same triangle.
        (
                ( rightData.GetObject () != leftData.GetObject () )
        ) || (
                ( rightData.GetObject () == leftData.GetObject () )
            &&  ( rightData.GetTriangleIndex () != leftData.GetTriangleIndex () )
        )
    ) {
        const Vec3Df&  leftPoint =  leftData.GetIntersectionPoint();
        const Vec3Df& rightPoint = rightData.GetIntersectionPoint();

        double  leftDist = Vec3Df::distance (  leftPoint, iRay.getOrigin() );
        double rightDist = Vec3Df::distance ( rightPoint, iRay.getOrigin() );

        if (
            ( rightDist <= leftDist )
        ) {
            oIntersection = rightData;
        } else {
            oIntersection = leftData;
        }
    } else {
        oIntersection = rightData;
    }

    return true;
}
