// *********************************************************
// Light Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef LIGHT_H
#define LIGHT_H

#include <iostream>
#include <vector>
#include <random>
#include <ctime>

#include "Vec3D.h"
#include "ParameterHandler.h"

#define PI 3.14159265358979323846


class Light {
public:
    inline Light () : color (Vec3Df (1.0f, 1.0f, 1.0f)), intensity (1.0f) {}
    inline Light (const Vec3Df & pos, const Vec3Df & color, float intensity)
        : pos (pos), color (color), intensity (intensity) {}
    virtual ~Light () {}

    inline const Vec3Df & getPos () const { return pos; }
    inline const Vec3Df & getColor () const { return color; }
    inline float getIntensity () const { return intensity; }

    inline void setPos (const Vec3Df & p) { pos = p; }
    inline void setColor (const Vec3Df & c) { color = c; }
    inline void setIntensity (float i) { intensity = i; }
    
    inline void getSamples (
        const float&            iRadius,
        const unsigned int&     iNumSamples,
        const Vec3Df&           iNormal,
        std::vector< Light >&   oSamples
    ) const {
        if (
                ( iRadius <= 0.0f )
            ||  ( iNumSamples <= 1u )
        ) {
            oSamples.push_back (
                *this
            );
        } else {
            std::default_random_engine generator ( time ( NULL ) );
            
            std::uniform_real_distribution<float> distRadius ( 0.0f, iRadius );
            std::uniform_real_distribution<float> distTheta ( 0.0f, 2 * PI );
            
            Vec3Df xAxis, yAxis;
            iNormal.getTwoOrthogonals ( xAxis, yAxis );
            xAxis.normalize ();
            yAxis.normalize ();

            float sampleIntensity = getIntensity () / ( (float) iNumSamples );
            for ( unsigned int s = 0; s < iNumSamples; s++ ) {
                float radius = distRadius ( generator ); 
                float theta = distTheta ( generator );
               
                float x = radius * cos ( theta );
                float y = radius * sin ( theta );
                
                oSamples.push_back (
                    Light (
                        getPos () + x * xAxis + y * yAxis,
                        getColor (),
                        sampleIntensity
                    )
                ); 
            }                
        }
    }
    inline void getSamples (
        const float&            iRadius,
        const unsigned int&     iNumSamples,
        const Vec3Df&           iNormal,
        std::vector< Vec3Df >&  oSamples
    ) const {
        if (
                ( iRadius <= 0.0f )
            ||  ( iNumSamples <= 1u )
        ) {
            oSamples.push_back (
                getPos ()
            );
        } else {
            std::default_random_engine generator ( rand () );// ( time ( NULL ) );
            
            std::uniform_real_distribution<float> distRadius ( 0.0f, iRadius );
            std::uniform_real_distribution<float> distTheta ( 0.0f, 2 * PI );
            
            Vec3Df xAxis, yAxis;
            iNormal.getTwoOrthogonals ( xAxis, yAxis );
            xAxis.normalize ();
            yAxis.normalize ();

            for ( unsigned int s = 0; s < iNumSamples; s++ ) {
                float radius = distRadius ( generator ); 
                float theta = distTheta ( generator );
               
                float x = radius * cos ( theta );
                float y = radius * sin ( theta );
                
                oSamples.push_back (
                    getPos () + x * xAxis + y * yAxis
                ); 
            }                
        }
    }
    
private:
    Vec3Df pos;
    Vec3Df color;
    float intensity;
};

#endif // LIGHT_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
