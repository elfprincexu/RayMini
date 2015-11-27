#include "Surfel.h"

Surfel::Surfel (
    const Material&                 iMaterial,
    const std::vector< Vertex >&    iVertexList,
    const Triangle&                 iTriangle,
    const Vec3Df&                   iTranslation
)   :   m_material ( &iMaterial ),
        m_color ( 0.0f, 0.0f, 0.0f )
{
    const Vertex& iA = iVertexList[iTriangle.getVertex(0)];
    const Vertex& iB = iVertexList[iTriangle.getVertex(1)];
    const Vertex& iC = iVertexList[iTriangle.getVertex(2)];

    BuildFromVertices (
        iA,
        iB,
        iC,
        iTranslation
    );
}

Surfel::Surfel (
    const Material&     iMaterial,
    const Vec3Df&       iPosition,
    const Vec3Df&       iNormal,
    const float&        iRadius,
    const Vec3Df&       iColor
)   :   m_radius ( iRadius ),
        m_position ( iPosition ),
        m_normal ( iNormal ),
        m_material ( &iMaterial ),
        m_color ( iColor )
{}

Surfel::Surfel (
    const Material&     iMaterial,
    const Vertex&       iA,
    const Vertex&       iB,
    const Vertex&       iC
)   :   m_material ( &iMaterial ),
        m_color ( 0.0f, 0.0f, 0.0f )
{
    BuildFromVertices (
        iA,
        iB,
        iC
    );
}

bool Surfel::Contains (
    const Vec3Df&   iPoint
) const {
    float distance = ( iPoint - GetPosition () ).getLength ();

    return ( distance <= GetRadius () );
}

void Surfel::BuildFromVertices (
    const Vertex&       iA,
    const Vertex&       iB,
    const Vertex&       iC,
    const Vec3Df&       iTranslation
) {
    // Edge eX is opposed to vertex X.
    Vec3Df eA = iC.getPos () - iB.getPos ();
    Vec3Df eB = iC.getPos () - iA.getPos ();
    Vec3Df eC = iA.getPos () - iB.getPos ();

    float a = eA.getLength ();
    float b = eB.getLength ();
    float c = eC.getLength ();
    
    eA.normalize ();
    eB.normalize ();
    eC.normalize ();
   
    // The perimeter of the triangle.
    float p = (a + b + c);
    // The semiperimeter of the triangle.
    float s = 0.5f * p;
    // The area of the triangle.
    float k = sqrt ( s * ( s - a ) * ( s - b ) * ( s - c ) );
    
    // The surfel radius equals the radius of the circle inscribed
    // in the triangle defined by vertices iA, iB and iC.
    m_radius = k / s;

    // The surfel's position is defined by the center of the inscribed
    // circle, which is subsequently defined by the intersection point
    // of the angle bisections.
    m_position  = a * iA.getPos () 
                + b * iB.getPos () 
                + c * iC.getPos ();
    m_position /= p;
    m_position += iTranslation;

    // We interpolate the normals the same way.
    m_normal    = a * iA.getNormal () 
                + b * iB.getNormal () 
                + c * iC.getNormal ();
    m_normal   /= p;
}
