#include "kd/KdLeafNode.h"

using namespace kd;

/*!
 * \inheaderfile
 */
bool KdLeafNode::Intersect (
    const Ray&              iRay,
    KdIntersectionData&     oIntersection,
    const float&            iNear,
    const float&            iFar
) const {
    // Stores whether or not an intersection has occurred.
    bool intersects = false;

    // Smallest distance found so far.
    float minDist = std::numeric_limits<float>::infinity();

    // For each primitive contained in the region described by the node,
    // test for intersection.
    for (
         KdDataVector::const_iterator it = m_elems.begin();
         it != m_elems.end();
         it++
    ) {
        // Obtains the primitive to test.
        KdData& primitive = *(*it);

        // The primitive's vertices.
        const Vertex&   v0  =   primitive[0];
        const Vertex&   v1  =   primitive[1];
        const Vertex&   v2  =   primitive[2];

        // Tests for ray-triangle intersection.
        float t, u, v;
        if (
            iRay.intersect (
                v0,
                v1,
                v2,
                t,
                u,
                v
            )
        ) {
            // The intersection point occurs at R(t) = O + t * d
            // where t is the distance from the ray's origin along it's
            // direction d.
            Vec3Df point  = iRay.getOrigin () + t * iRay.getDirection ();

            // The normal at the intersection point is the barycentric interpolation
            // of the normals of all vertices in the triangle.
            Vec3Df normal = ( 1 - u - v ) * v0.getNormal ()
                          +       u       * v1.getNormal ()
                          +       v       * v2.getNormal ();
            normal.normalize ();

            //  If the distance t is between the minimum and maximum distances   AND
            //  If the distance t is smaller than the smallest distance found    AND
            //  If the triangle is not backfacing the ray.
            if (
                    ( t >= iNear )
                &&  ( t <= iFar )
                &&  ( t < minDist )
                &&  ( Vec3Df::dotProduct ( normal, iRay.getDirection () ) < 0.0f )
            ) {
                // Calculates the bumped normal at intersection point.
                normal = primitive.GetObject ()->getBumpedNormal (
                    primitive.GetTriangleIndex (),
                    u,
                    v
                );

                minDist = t;

                // Stores intersection data.
                oIntersection = KdIntersectionData (
                    &primitive,
                    normal,
                    point,
                    t,
                    u,
                    v
                );

                // Signals an intersection.
                intersects = true;
            }
        }
    }

    // Returns whether or not an intersection has been found.
    return intersects;
}

/*!
 * \inheaderfile
 */
bool KdLeafNode::IntersectSurfel (
    const Ray&              iRay,
    KdIntersectionData&     oIntersection,
    const float&            iNear,
    const float&            iFar
) const {
    // Stores whether or not an intersection has occurred.
    bool intersects = false;

    // Smallest distance found so far.
    float minDist = std::numeric_limits<float>::infinity();

    // For each primitive contained in the region described by the node,
    // test for intersection.
    for (
         KdDataVector::const_iterator it = m_elems.begin();
         it != m_elems.end();
         it++
    ) {
        // Obtains the primitive to test.
        KdData& primitive = *(*it);

        // The intersection point with the primitive's surfel.
        Vec3Df intersectionPoint;

        // Tests for ray-surfel intersection.
        if (
            iRay.intersect (
                primitive.GetSurfel (),
                intersectionPoint
            )
        ) {
            // Calculates the distance from the ray's origin.
            float t = ( intersectionPoint - iRay.getOrigin () ).getLength ();

            // The intersection normal is the intersected surfel's normal.
            const Vec3Df& normal = primitive.GetSurfel ().GetNormal ();

            //  If the distance t is between the minimum and maximum distances   AND
            //  If the distance t is smaller than the smallest distance found    AND
            //  If the triangle is not backfacing the ray.
            if (
                    ( t >= iNear )
                &&  ( t <= iFar )
                &&  ( t < minDist )
                &&  ( Vec3Df::dotProduct ( normal, iRay.getDirection () ) < 0.0f )
            ) {
                minDist = t;

                // Stored the intersection data.
                oIntersection = KdIntersectionData (
                    &primitive,
                    normal,
                    intersectionPoint,
                    t,
                    0.0f,
                    0.0f
                );

                // Signals an intersection.
                intersects = true;
            }
        }
    }

    // Returns whether or not an intersection has been found.
    return intersects;
}
