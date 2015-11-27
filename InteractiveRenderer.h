#ifndef INTERACTIVERENDERER_H
#define INTERACTIVERENDERER_H

#include <QThread>
#include <QImage>
#include "Vec3D.h"
#include "GLViewer.h"
#include <QMutex>
#include <QMutexLocker>

//macros
#define RESET_INTERACTIVITY_BEGIN \
    RayTracer::getInstance()->getInterRenderer().cancel(); \
    RayTracer::getInstance()->getInterRenderer().lock();

#define RESET_INTERACTIVITY_END \
    RayTracer::getInstance()->getInterRenderer().unlock();

/*!
 * \brief The InteractiveRenderer class implements interactive rendering thread.
 */
class InteractiveRenderer: public QThread {
    friend class RayTracer;
public:

    /*!
     * \brief Launches interactive rendering.
     * \param viewer    QGLViewer instance used to display the rendered content.
     */
    void begin(GLViewer* viewer);

    /*!
     * \brief Quitting interactive mode.
     */
    inline void disable() { fEnabled = false; }

    /*!
     * \brief Aborts current rendering.
     * Used when a parameter change occurs. Causes current rendering break and waits until the
     * thread stops. All the rendered content will be freed.
     */
    void cancel();

    /*!
     * \brief Interactive mode flag.
     * \return `true` if the interactive mode is considered to be enabled by the renderer.
     */
    inline bool isEnabled() const { return fEnabled; }

    /*!
     * \brief Gives actually rendered image.
     * This routine must be called when the renderer is in locked state.
     * \return QImage object pointer contained actually rendered image.
     */
    inline QImage* getImage() const { return fResult; }

    /*!
     * \brief Frames per second value as interactivity speed indicator.
     * \return frames per second value.
     */
    inline float getFPS() const { return fFPS; }

    /*!
     * \brief Retrieving information about interactivity rendering process.
     * \return different information concerning the process, basicly actual image construction
     * or anti-aliasing progress.
     */
    const QString getStatus() const;

    /*!
     * \brief Locks renderer state signifying entering into critical section protecting rendering parameters.
     */
    inline void lock() { fMutex.lock(); }

    /*!
     * \brief Unlocks renderer state after locking it by InteractiveRenderer::lock().
     */
    inline void unlock() { fMutex.unlock(); }

    /*!
     * \brief Class constructor.
     */
    InteractiveRenderer();

    /*!
    * \brief Class destructor.
    */
    ~InteractiveRenderer();

protected:
    void run();
    inline bool wasCancelled() const { return fCancelled; }

private:
    float getXOffset() const;       //!< Downsampled image horizontal offset in pixels
    float getYOffset() const;       //!< Downsampled image vertical offset in pixels

    static const int SUBDIVISION = 8;   //!< Downsampling factor

    GLViewer *fViewer;              //!< Rendering target
    QImage
        *fRenderedStock,            //!< Stock of rendered content
        *fResult;                   //!< Resulted image
    QMutex fMutex;                  //!< Internal instance data access control
    bool
        fEnabled,                   //!< `true` if interactive mode is enabled
        fCancelled;                 //!< `true` if the rendering is aborted (because of parameter change)
    unsigned int fPass;             //!< Number of pass of image construction. Controls instance state.
    float fFPS;                     //!< Frames per second (in fact, inversed time of last rendered image)

    Vec3Df
        fCamPos,                    //!< Current camera position
        fUpVector;                  //!< Current camera up vector
    unsigned int
        fScreenWidth,               //!< Current width of the rendering target in pixels
        fScreenHeight;              //!< Current height of the rendering target in pixels
    float fFOV;                     //!< Current field of view value

    unsigned int
        fSmpX [SUBDIVISION * SUBDIVISION],  //!< Donwsampled image horizontal offset in function of \var fPass
        fSmpY [SUBDIVISION * SUBDIVISION];  //!< Donwsampled image vertical offset in function of \var fPass
};

#endif // INTERACTIVERENDERER_H
