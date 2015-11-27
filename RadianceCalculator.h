#ifndef _RADIANCECALCULATOR_H_
#define _RADIANCECALCULATOR_H_

#include "Material.h"
#include "Vec3D.h"
#include "Scene.h"
#include "Ray.h"
#include "MathUtils.h"

/*!
 *  \brief  Singleton class that contains all the functions needed to calculate
 *          the radiance of a point in space.
 */
class RadianceCalculator {
private:
    // Private constructors and destructors to prevent creation and destruction
    // of singleton objects outside of class. 
    RadianceCalculator () {}
    ~RadianceCalculator () {}

    // Private copy constructor and affection operator to prevent copies of the
    // singleton object.    
    RadianceCalculator ( const RadianceCalculator& ) {}
    RadianceCalculator& operator= ( const RadianceCalculator& ) {
        return *this;
    }

public:
    /*!
     *  \brief  Returns the address of the singleton object.
     */
    static inline RadianceCalculator* Instance ()
    {
        // Static instance of the RadianceCalculator class.
        static RadianceCalculator _instance;
        return &_instance;
    }

    /*!
     *  \brief  Returns the ambient occlusion value of a point P inside a given scene.
     *
     *  Casts N rays distributed over a hemisphere around the normal of the surface containint P
     *  and counts how many of them hit the surrounding geometry with maximum distance R.
     *  
     *  \param  iRayCount   Number of samples.
     *  \param  iRadius     Maximum occluding object's distance.
     *  \param  iNormal     The normal of the surface containing P, on P.
     *  \param  iPoint      The point P.
     *  \param  iScene      The scene descriptor.
     *  \return The ratio of rays intersection the surrounding geometry.
     */
    inline float AmbientOcclusion (
        const int&      iRayCount,
        const float&    iRadius,
        const Vec3Df&   iNormal,
        const Vec3Df&   iPoint,
        const Scene&    iScene
    ) const {
        // The KD-Tree description of the scene.
        const KdTree& kt = *( iScene.getKdTree () );

        // The number of intersections.
        int nbIntersection = 0;

        // Random uniform number generators for spherical coordinates
        // Radius and Theta of the point on the unit sphere where
        // a ray should be cast.
        std::default_random_engine generator ( rand () );
        std::uniform_real_distribution<float> distributionRadius(0,1);
        std::uniform_real_distribution<float> distributionTheta(0,2*M_PI);

        // Casting of rays.
        for (int i = 0; i < iRayCount; i++){

            const float radius = distributionRadius(generator);
            const float tetha  = distributionTheta(generator);

            // Point in upper hemisphere, with cosine distribution.
            Vec3Df rnd = CosineWeightedDistribution(radius,tetha) ;
            rnd.normalize();

            // Orthonormal basis created from the surface's normal.
            //  z is perpendicuar to the surface on P.
	        //  x, y are perpendicular to it and between themselves.
            const Vec3Df& z = iNormal;
            Vec3Df axis;
            if ( z[0] < z[1] && z[0] < z[2] )
            {  axis = Vec3Df ( 1.0f, 0.0f, 0.0f ); }
            else if ( z[1] < z[2] )
            { axis = Vec3Df ( 0.0f, 1.0f, 0.0f ); }
            else
            { axis = Vec3Df ( 0.0f, 0.0f, 1.0f ); }
            Vec3Df x = Vec3Df::crossProduct ( z, axis );
            Vec3Df y = Vec3Df::crossProduct ( z, x );

            // We perform a basis change to express our random point on the sphere
            // on the coordinate system expressing the hemisphere around the point P.
            Vec3Df refDir = (x * rnd[0]) + (y * rnd[1]) + (z * rnd[2]);
            refDir.normalize();

            // DEBUG: Verify if the calculated direction is not opposite to the hemisphere.
            if (
                Vec3Df::dotProduct ( z, refDir ) < 0.f
            ) {
                std::cout << "ERROR: AO Ray Position in wrong direction!!" << std::endl;
            }

            // Intersection descriptor.
            KdIntersectionData intersectionData;
            // If the ray intersects the geometry, increase counter.
            if (
                kt.Intersect (
                    Ray (
                        iPoint,
                        refDir
                    ),
                    intersectionData,
                    EPSILON,
                    iRadius
                )
            ) {
                    nbIntersection += 1;
            }
        }

        return (float)nbIntersection/iRayCount;
    }

    /*! \brief  Returns the contribution of a single light to the intensity of the 
     *          Phong BRDF applied to a point on a given surface.
     * 
     *  Calculates the contribution of a single point light to the intensity of the BRDF at a given point,
     *  by using Phong's formula \f[ I_d = k_d * ( L_m * N ) * i_d + k_s * ( R_m * V ) ^ \alpha * i_s \f]
     *
     *  \param  iPoint      The point where to calculate the BRDF.
     *  \param  iNormal     The normal of the surface containing iPoint.
     *  \param  iViewPoint  The location of the observer.
     *  \param  iLightPos   The position of the light that illuminates the point.
     *  \param  iLightColor The RGB color intensity of the light that illuminates the point.
     *  \param  iMaterial   The material description of the surface (specular and diffuse coefficients, color, etc).
     *  \return             The RGB value of the light's Phong contribution to the BRDF.
     */
    inline Vec3Df Phong (
        const Vec3Df&   iPoint,
        const Vec3Df&   iNormal,
        const Vec3Df&   iViewPoint,
        const Vec3Df&   iLightPos,
        const Vec3Df&   iLightColor,
        const Material& iMaterial
    ) const {
        // The material color.
        const Vec3Df& matColor = iMaterial.getColor ();

        // Diffuse and specular contributions to the BRDF.
        Vec3Df diffuse ( 0.0f, 0.0f, 0.0f );
        Vec3Df specular ( 0.0f, 0.0f, 0.0f );

        // Direction pointing to the light.
        Vec3Df          incident    = iLightPos - iPoint;
        incident.normalize();

        // The perfect reflection direction.
        Vec3Df          reflected   = incident - 2 * iNormal * Vec3Df::dotProduct ( incident, iNormal );
        reflected.normalize();

        // Diffuse coefficient of the material, in RGB.
        const Vec3Df    kd      = iMaterial.getDiffuse() * matColor;

        // Specular coefficient of the material, in RGB.
        const Vec3Df    ks      = iMaterial.getSpecular() * matColor;

        // The angle of incident light rays.
        const float     incAng  = Vec3Df::dotProduct ( incident, iNormal );

        // If incident light rays are no 
        if (
            incAng > 0.0f
        ) {
            // The diffuse contribution to the BRDF. 
            diffuse = kd * iLightColor * incAng;

            // The direction pointing to the observer.
            Vec3Df view = iViewPoint - iPoint;
            view.normalize();
            
            // The angle between the view direction and the reflection direction.
            float camViewAng = Vec3Df::dotProduct ( reflected, view );

            // If observer is not on the opposite direction of the perfect reflection ray,
            // calculate the specular contribution.
            if (
                camViewAng > 0.0f
            ) {
                // The shininess of the material.
                const float& shininess  = iMaterial.getShininess ();
                
                // The specular contribution to the BRDF.
                specular    = ks
                            * iLightColor
                            * pow ( camViewAng, shininess );
            }
        }

        return ( diffuse + specular );
    }
    
    /*! 
     *  \brief  Calculates the visibility function of a point T from a point P inside a given scene.
     *
     *  Casts a ray from a point P inside a scene to another point T inside the same scene
     *  and queries the scene's KD-Tree for an intersection. If no intersection exists,
     *  the point T is visible from P's position.
     *
     *  \param  iScene          The scene description.
     *  \param  iFromPoint      The point P.
     *  \param  iTargetPoint    The point T.
     *  \return true if the ray cast from P to T doesn't intersect the scene in between them.
     */        
    inline bool PointVisibility (
        const Scene&        iScene,
        const Vec3Df&       iFromPoint,
        const Vec3Df&       iTargetPoint
    ) const {
        // The KD-Tree description of the scene.
        const KdTree* kdTree = iScene.getKdTree ();
        // Stores the intersection data of the ray cast from P to T.
        KdIntersectionData intersection;

        // The direction of the ray to cast.
        Vec3Df shadowRayDir = iTargetPoint - iFromPoint;

        // To avoid intersections with the point of origin of the ray.
        float nearPlane =   0.000000000000000000005f;
        
        // Intersections should only be considered if they happened on a distance
        // smaller than that of T to P.
        float farPlane  =   shadowRayDir.normalize ();

        // Casts the ray.
        bool intersects = kdTree->Intersect (
            Ray (
                iFromPoint,
                shadowRayDir
            ),
            intersection,
            nearPlane,
            farPlane
        );

        return ( !intersects );
    }

    /*!
     *  \brief  Calculates the visibility function of a set S of points from a point P inside a given scene.
     *  
     *  \param  iScene      The scene descriptor.
     *  \param  iPoint      The point P.
     *  \param  iPointSet   The set S of points.
     *  \return The fraction of points from S that can be seen from P. 
     */
    inline float PointSetVisibility (
        const Scene&                iScene,
        const Vec3Df&               iPoint,
        const std::vector< Vec3Df > iPointSet
    ) const {
        // Counter for the number of points in S that are visible from P.
        unsigned int visible = 0u;
        
        // For every point in the set S, cast a ray from P towards it.
        for (
            std::vector< Vec3Df >::const_iterator it = iPointSet.begin ();
            it != iPointSet.end ();
            it++
        ) {
            // If a point is visible from P, increment the counter.
            if (
                PointVisibility (
                    iScene,
                    iPoint,
                    *it
                )
            ) {
                visible++;
            }
        }

        // The visibility of the point set is the fraction of rays cast
        // towards it that didn't intersect the scene's geometry.
        return ( (float) visible ) / ( (float) iPointSet.size () );
    }

    /*!
     *  \brief  Determines the visibility function for a light source from
     *          a point P inside a scene.
     *  
     *  \param  iScene      The scene descriptor.
     *  \param  iPoint      The point P.
     *  \param  iLight      The light source.
     */
    inline float LightVisibility (
        const Scene&                iScene,
        const Vec3Df&               iPoint,
        const Light&                iLight
    ) const {
        // Gets the instance of the parameter handler.
        const ParameterHandler* params = ParameterHandler::Instance();

        // Visibility if 100% unless shadows are enabled.
        float v = 1.0f;

        // If shadows are enabled.
        if ( params->GetShadows () ) {
            // If soft shadows are enabled, consider all light sources
            // as extended.
            if ( params->GetSoftShadows () ) {
                std::vector< Vec3Df > lightSamples;

                // Get a set of sample points from the light's
                // area.
                iLight.getSamples (
                    params->GetLightRadius (),
                    params->GetLightSamples (),
                    Vec3Df ( 0.0f, -1.0f, 0.0f ),
                    lightSamples
                );

                // Calculate the visibility of the set of sample points.
                v = PointSetVisibility (
                    iScene,
                    iPoint,
                    lightSamples
                );
            } else {
                // The visibility of the point light is the visibility
                // of the point where it's positioned.
                if (
                    !PointVisibility (
                        iScene,
                        iPoint,
                        iLight.getPos ()
                    )
                ) {
                    v = 0.0f;
                }
            }
        }
        
        // Return the visibility.
        return v;
    }

    /*!
     *  \brief  Calculates the direct illumination of a point P contained in a given scene,
     *          from the point of view of an observer O.
     *
     *  For all the light sources inside the scene, calculate their visibility from
     *  their Phong contribution to the radiance on that point. Direct lighting is then
     *  the sum of all contributions modulated by their visibility.
     *
     *  \param  iScene      The scene descriptor.
     *  \param  iViewPoint  The observer's location on the scene.
     *  \param  iPoint      The point P.
     *  \param  iNormal     The normal of P's containing surface at P.
     *  \param  iMaterial   The material of the surface containing P.
     *  \return The total radiance from all direct light sources on P.
     */
    inline Vec3Df DirectLighting (
        const Scene&    iScene,
        const Vec3Df&   iViewPoint,
        const Vec3Df&   iPoint,
        const Vec3Df&   iNormal,
        const Material& iMaterial
    ) const {
        // Vector of light sources.
        const std::vector< Light >& sceneLights = iScene.getLights ();

        // The color of the point P.
        Vec3Df color ( 0.0f, 0.0f, 0.0f );
       
        // For all light sources in the scene, add their Phong contribution to the color
        // of the point P.
        for (
            std::vector< Light >::const_iterator light = sceneLights.begin();
            light != sceneLights.end();
            light++
        ) {
            // Calculate the light's visibility v.
            float v = LightVisibility (
                iScene,
                iPoint,
                *light
            );

            // Modulate the Phong contribution by v. 
            color += v * Phong (
                iPoint,
                iNormal,
                iViewPoint, 
                light->getPos (),
                light->getColor () * light->getIntensity (),
                iMaterial
            );
        }

        return color;
    }

};

#endif // _RADIANCECALCULATOR_H_
