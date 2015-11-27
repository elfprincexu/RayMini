#include "InteractiveRenderer.h"
#include "Vec3D.h"
#include "RayTracer.h"

#define sqr(x) ((x)*(x))
#define SUB2 sqr(SUBDIVISION)

inline int closestPowOf2(int X) {
    int p = 0;
    for (p = 0; 1 << (p+1) <= X; p++);
    return 1 << p;
}


float InteractiveRenderer::getXOffset() const {
    return 1.0f * fSmpX[ fPass % SUB2 ] / SUBDIVISION
         + 1.0f * fSmpX[ (fPass / SUB2) % SUB2 ] / SUB2;
}


float InteractiveRenderer::getYOffset() const {
    return 1.0f * fSmpY[ fPass % SUB2 ] / SUBDIVISION
         + 1.0f * fSmpY[ (fPass / SUB2) % SUB2 ] / SUB2;
}

InteractiveRenderer::InteractiveRenderer():
    fViewer(NULL),fRenderedStock(NULL), fResult(NULL),fEnabled(false), fCancelled(false), fPass(0), fFPS(0.0f),
    fCamPos(), fUpVector(), fScreenWidth(0), fScreenHeight(0), fFOV(0.0f)
{
    //sampling index table initialization
    int x = 0, y = 0;
    unsigned int step = 2 * SUBDIVISION;
    bool odd = true;
    for (unsigned int i = 0; i < sqr(SUBDIVISION); i++) {
        fSmpX[i] = x;
        fSmpY[i] = y;
        if (odd)
            x += step;
        else
            x += step/2;
        if (x >= SUBDIVISION) {
            y += step/2;
            if (y >= SUBDIVISION) {
                step /= 2;
                y = 0;
                x = step/2;
                odd = true;
            } else {
                odd = !odd;
                if (odd)
                    x = step/2;
                else
                    x = 0;
            }
        }
    }
}


InteractiveRenderer::~InteractiveRenderer() {
    cancel();
    if (fRenderedStock)
        delete fRenderedStock;
    if (fResult)
        delete fResult;
}


void InteractiveRenderer::run() {
    qglviewer::Camera * cam = fViewer->camera ();
    qglviewer::Vec p = cam->position ();
    qglviewer::Vec d = cam->viewDirection ();
    qglviewer::Vec u = cam->upVector ();
    qglviewer::Vec r = cam->rightVector ();
//      qglviewer::Vec p(0.0,0.0,0.0);
//      qglviewer::Vec d(0.0,0.0,-1.0);
//      qglviewer::Vec u(0.0,1.0,0.0);
//      qglviewer::Vec r (1.0,0.0,0.0);
    Vec3Df camPos (p[0], p[1], p[2]);
    Vec3Df viewDirection (d[0], d[1], d[2]);
    Vec3Df upVector (u[0], u[1], u[2]);
    Vec3Df rightVector (r[0], r[1], r[2]);
    float fieldOfView = cam->fieldOfView ();
    float aspectRatio = cam->aspectRatio ();
    unsigned int screenWidth = cam->screenWidth ();
    unsigned int screenHeight = cam->screenHeight ();

    //locking access control
    lock();

    //reset, if necessary
    if (
        fCancelled ||
        fCamPos != camPos  ||  fUpVector != upVector  ||  fFOV != fieldOfView  ||
        fScreenWidth != screenWidth  ||  fScreenHeight != screenHeight
    ) {
        if (fRenderedStock)
            delete fRenderedStock;
        if (fResult)
            delete fResult;
        fRenderedStock = NULL;
        fResult = NULL;
        fPass = 0;
    }
    fCamPos = camPos;
    fUpVector = upVector;
    fFOV = fieldOfView;
    fScreenWidth = screenWidth;
    fScreenHeight = screenHeight;

    //initializing the images if needed
    if (!fRenderedStock)
        fRenderedStock = new QImage(
            QSize(fViewer->camera()->screenWidth(), fViewer->camera()->screenHeight()),
            QImage::Format_RGB888
        );
    if (!fResult) {
        fResult = new QImage(
            QSize(fViewer->camera()->screenWidth(), fViewer->camera()->screenHeight()),
            QImage::Format_RGB888
        );
        fResult->fill(qRgb(0,0,0));
    }

    //launch rendering
    fCancelled = false;
    QTime timer;
    timer.start ();
    QImage img = RayTracer::getInstance()->render (
        camPos, viewDirection, upVector, rightVector, fieldOfView, aspectRatio,
        screenWidth  / SUBDIVISION + 1,
        screenHeight / SUBDIVISION + 1
    );

    //checking if aborted
    if (fCancelled) {
        fResult->fill(qRgb(20,20,20));
        unlock();
        return;
    }

    unsigned int WholePass = (fPass+1) / sqr(SUBDIVISION);

    int meaningCellSize = max(1, SUBDIVISION / closestPowOf2( (int) floorf(sqrtf(fPass+1)) ) );
    int fillingCellSize = SUBDIVISION;
    if (fPass > 0)
        fillingCellSize = max(1, SUBDIVISION / 2 / closestPowOf2( (int) floorf(sqrtf(fPass)) ) );

    //Stocking new data
    if (fPass < sqr(SUBDIVISION))
    {
        //Full resolution image is not constructed yet.
        for (int cx = 0; cx < img.width(); cx++)
            for (int cy = 0; cy < img.height(); cy++) {
                QRgb pix = img.pixel(cx, cy);
                //Referring to full resolution image (cx->x, cy->y)
                int
                    x = cx * SUBDIVISION + fSmpX[ fPass % sqr(SUBDIVISION) ],
                    y = cy * SUBDIVISION + fSmpY[ fPass % sqr(SUBDIVISION) ];
                //Filling square region in fRenderedStock by new pixel
                for (int i = x; i < min(x+fillingCellSize, fRenderedStock->width()); i++)
                    for (int j = y; j < min(y+fillingCellSize, fRenderedStock->height()); j++)
                        fRenderedStock->setPixel(i, j, pix);
            }
    } else
    {
        //Full resolution image is already constructed: anti-aliasing.
        for (int cx = 0; cx < img.width(); cx++)
            for (int cy = 0; cy < img.height(); cy++) {
                QRgb pix = img.pixel(cx, cy);
                //Referring to full resolution image (cx->x, cy->y)
                int
                    x = cx * SUBDIVISION + fSmpX[ fPass % sqr(SUBDIVISION) ],
                    y = cy * SUBDIVISION + fSmpY[ fPass % sqr(SUBDIVISION) ];
                //Correcting stocked pixel color by a new value
                if (x < fRenderedStock->width()  &&  y < fRenderedStock->height()) {
                    QRgb prev = fRenderedStock->pixel(x, y);
                    fRenderedStock->setPixel(
                        x, y,
                        qRgb(               //(quantization effect is negliged here)
                            (qRed(prev)   * WholePass + qRed(pix)   ) / (WholePass+1),
                            (qGreen(prev) * WholePass + qGreen(pix) ) / (WholePass+1),
                            (qBlue(prev)  * WholePass + qBlue(pix)  ) / (WholePass+1)
                        )
                    );
                }
            }
    }

    //Output image constructing
    for (unsigned int cx = 0; cx < screenWidth; cx += meaningCellSize)
        for (unsigned int cy = 0; cy < screenHeight; cy += meaningCellSize) {
            int
                sx = min(cx + meaningCellSize, screenWidth),
                sy = min(cy + meaningCellSize, screenHeight);

            //Output value computing locally averaging pixels stocked in fRenderedStock
            unsigned int N = 0;
            unsigned int rVal = 0, gVal = 0, bVal = 0;
            for (int i = cx; i < sx; i++)
                for (int j = cy; j < sy; j++) {
                    QRgb c = fRenderedStock->pixel(i,j);
                    rVal += qRed(c);
                    gVal += qGreen(c);
                    bVal += qBlue(c);
                    N++;
                }
            QRgb out = qRgb(rVal / N, gVal / N, bVal / N);

            //Filling up output image
            for (int i = cx; i < sx; i++)
                for (int j = cy; j < sy; j++)
                    fResult->setPixel(i, j, out);
        }

    //Increasing pass counter
    fPass++;

    //FPS computing
    if (timer.elapsed() > 0)
        fFPS = 1000.0f / timer.elapsed();

    unlock();
}


void InteractiveRenderer::begin(GLViewer* viewer) {
    cancel();
    lock();
    fViewer = viewer;
    fViewer->noAutoOpenGLDisplayMode = true;
    fEnabled = true;
    unlock();
    start();
}


void InteractiveRenderer::cancel() {
    fCancelled = true;
    wait();
}


const QString InteractiveRenderer::getStatus() const {
    unsigned int WholePass = (fPass+1) / sqr(SUBDIVISION);
    if (WholePass == 0)
        return QString("image construction: ") +
               QString::number(100 * fPass / sqr(SUBDIVISION)) + QString("%");
    else
        return QString("anti-aliasing (") + QString::number(WholePass) + QString(" rays): ") +
               QString::number(100 * (fPass % sqr(SUBDIVISION)) / sqr(SUBDIVISION)) + QString("%");
}
