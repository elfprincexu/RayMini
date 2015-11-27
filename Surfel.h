#ifndef _SURFEL_H_
#define _SURFEL_H_

#include "Vec3D.h"
#include "Material.h"
#include "Vertex.h"
#include "Triangle.h"

// Defines a surface element containing a position in 3D space, the
// normal to its containing plane, a radius, a material and a color.
class Surfel {
private:
    float           m_radius;
    Vec3Df          m_position;
    Vec3Df          m_normal;
    const Material* m_material;
    Vec3Df          m_color;

private:
    // Builds a surfel from its enclosing triangle's vertices.
    void BuildFromVertices (
        const Vertex&       iA,
        const Vertex&       iB,
        const Vertex&       iC,
        const Vec3Df&       iTranslation=Vec3Df(0.0f, 0.0f, 0.0f)
    );

public:
    // Default ctor.
    Surfel () {}
    // Creates a surfel from a triangle.
    Surfel (
        const Material&                 iMaterial,
        const std::vector< Vertex >&    iVertexList,
        const Triangle&                 iTriangle,
        const Vec3Df&                   iTranslation=Vec3Df(0.0f, 0.0f, 0.0f)
    );
    // Creates a surfel from its basic information.
    Surfel (
        const Material&     iMaterial,
        const Vec3Df&       iPosition,
        const Vec3Df&       iNormal,
        const float&        iRadius,
        const Vec3Df&       iColor=Vec3Df ( 0.0f, 0.0f, 0.0f )
    );
    // Creates a Surfel from the vertices of a triangle.
    Surfel (
        const Material&     iMaterial,
        const Vertex&       iA,
        const Vertex&       iB,
        const Vertex&       iC
    );
    // Default dtor.
    ~Surfel() {}

    // Tests whether or not a point is inside the surfel.
    bool Contains (
        const Vec3Df&   iPoint
    ) const;

    // Accessors
    inline const float& GetRadius ()
    const {
        return m_radius;
    }
    inline const Vec3Df& GetPosition ()
    const {
        return m_position;
    }
    inline const Vec3Df& GetNormal ()
    const {
        return m_normal;
    }
    inline const Material& GetMaterial ()
    const {
        return (*m_material);
    }
    
    inline const Vec3Df& GetColor ()
    const {
        return m_color;
    }
    inline void SetColor (
        const Vec3Df&   iNewColor
    ) {
        m_color = iNewColor;
    }
};

#endif // _SURFEL_H_
