#ifndef _MATHUTILS_H_
#define _MATHUTILS_H_

#include "Vec3D.h"
#include "math.h"

inline double max (
    const float& iA,
    const float& iB
) {
    return ( ( iA > iB ) ? iA : iB );
}

inline double min (
    const float& iA,
    const float& iB
) {
    return ( ( iA < iB ) ? iA : iB );
}

template <typename T>
inline T max (
    const T& iA,
    const T& iB
) {
    return ( ( iA > iB ) ? iA : iB );
}

template <typename T>
inline T min (
    const T& iA,
    const T& iB
) {
    return ( ( iA < iB ) ? iA : iB );
}

inline int clamp (
    const float&    f,
    const int&      inf,
    const int&      sup
) {
    int v = static_cast<int> (f);
    return (v < inf ? inf : (v > sup ? sup : v));
}

inline float fclamp (
    const float&    iVal,
    const float&    iInf,
    const float&    iSup
) {
    return ( iVal < iInf ) ? iInf : ( iVal > iSup ) ? iSup : iVal;
}

template <typename T>
inline T tclamp (
    const T&    iVal,
    const T&    iInf,
    const T&    iSup
) {
    return ( iVal < iInf ) ? iInf : ( iVal > iSup ) ? iSup : iVal;
}


//! Generates a random vector using a cosine weighted distribution
/*! Objective: creates a disk (polar coordinate)
    and project it onto a hemisphere of unit size
    \param R      a random radius value
    \param iTheta a random iTheta value
*/
inline Vec3Df CosineWeightedDistribution (
    const float& iR,
    const float& iTheta
) {
    /* Projecting my disk onto a unit hemisphere
        _
       ( )
      (  /)  | Z
     (__/__) |
         R
    1 = R*R + Z*Z
    Z = sqrt(1 - R*R)
    */

    /* Converting between polar and Cartesian coordinates*/
    const float z = sqrt(1.0f - iR * iR);
    const float x = iR * cos ( iTheta );
    const float y = iR * sin ( iTheta );

    return Vec3Df (
        x,
        y,
        z
    );
}

#endif // _MATHUTILS_H_
