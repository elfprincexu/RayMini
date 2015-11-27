/*
 * Modified guided image filter implementation for DOF simulation
 * Based on publication of K. He "Guded image filtering"
 * Author: M. Karpushin
 */

#ifndef GUIDEDFILTER_H
#define GUIDEDFILTER_H

#include <qimage.h>

/*!
 * \brief The GuidedFilter class provides Guided filter implementation
 */
class GuidedFilter {
public:
    /*!
     * \brief Filter instance constructor.
     * \param imgWidth      Width in pixels of scene depth map and images will be filtered.
     * \param imgHeight     Height in pixels of scene depth map and images will be filtered.
     */
    GuidedFilter(unsigned int imgWidth, unsigned int imgHeight);

    /*!
     * \brief Filter instance destructor.
     */
    virtual ~GuidedFilter();

    /*!
     * \brief Depth map filling routine.
     * \param x     Pixel horizontal coordinate, must not exceed the scene size specified for this instance.
     * \param y     Pixel vertical coordinate, must not exceed the scene size specified for this instance.
     * \param val   Distance to object presenting at the given pixel.
     */
    inline void setDistance(unsigned int x, unsigned int y, float val) {
        fDistance[x][y] = val;
    }

    /*!
     * \brief Filter parameters initialization
     */
    inline void setDefaultEffectParameters() {
        fFocalPlanePos = 1;   //distance between the camera and the sharpest object; can be adjusted automatically by adjustFocalPlane()
        fDOFFactor = 1.0f;    //depth of view
    }

    /*!
     * \brief Calculates focal plane position in function of scene depth map.
     * Tries to focus on an object in the screen scenter. Should be called when the source image is rendered.
     */
    void adjustFocalPlane();

    /*!
     * \brief Applies filtering to the image in function of the depth map, focal plane position and effect parameters.
     * \param image     The image to be filtered.
     */
    void apply(QImage& image);

    /*!
     * \brief Resets depth map.
     */
    void resetDistances();

private:

    unsigned int fImgWidth, fImgHeight;     //!< Scene size

    float ** fDistance;                     //!< Scene depth map

    unsigned int fHalfWndSize;              //!< Half of filter rectangular window size

    float
        fFocalPlanePos,             //!< Focal plane position in scene space units on the camera view axis.
        fDOFFactor,                 //!< Depth of field factor.
        fRegularization;            //!< Filter regularization parameter (\epsilon).
};

#endif // GUIDEDFILTER_H
