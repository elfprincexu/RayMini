/*
 * Modified guided image filter implementation for DOF simulation
 * Based on publication of K. He "Guded image filtering"
 * Author: M. Karpushin
 */

#include "GuidedFilter.h"
#include <math.h>

#define sqr(x) ((x)*(x))

#ifndef M_PI
    #define M_PI		3.14159265358979323846
#endif

using namespace std;

GuidedFilter::GuidedFilter(unsigned int imgWidth, unsigned int imgHeight)
{
    fHalfWndSize = 4;
    fRegularization = 0.001f;
    setDefaultEffectParameters();
    fImgWidth = imgWidth;
    fImgHeight = imgHeight;
    fDistance = new float*[imgWidth];
    for (unsigned int x = 0; x < fImgWidth; x++)
        fDistance[x] = new float[imgHeight];
    resetDistances();
}


GuidedFilter::~GuidedFilter() {
    for (unsigned int x = 0; x < fImgWidth; x++)
        delete[] fDistance[x];
    delete[] fDistance;
}


void GuidedFilter::apply(QImage& image) {

    typedef double ftype;
    const unsigned int w = fImgWidth;
    const unsigned int h = fImgHeight;

    //arrays initialization
#define INIT(id) ftype ** id = new ftype*[w]
#define INIT_Y(id) id[x] = new ftype[h]
    INIT(sum);
    INIT(sec);
    INIT(Ip_r);
    INIT(Ip_g);
    INIT(Ip_b);
    INIT(p_r);
    INIT(p_g);
    INIT(p_b);
    INIT(a_r);
    INIT(a_g);
    INIT(a_b);
    INIT(b_r);
    INIT(b_g);
    INIT(b_b);
    INIT(distance);
    for (unsigned int x = 0; x < w; x++) {
        INIT_Y(sum);
        INIT_Y(sec);
        INIT_Y(Ip_r);
        INIT_Y(Ip_g);
        INIT_Y(Ip_b);
        INIT_Y(p_r);
        INIT_Y(p_g);
        INIT_Y(p_b);
        INIT_Y(a_r);
        INIT_Y(a_g);
        INIT_Y(a_b);
        INIT_Y(b_r);
        INIT_Y(b_g);
        INIT_Y(b_b);
        INIT_Y(distance);
    }

    //depth to guidance image mapping
    for (unsigned int x = 0; x < w; x++)
        for (unsigned int y = 0; y < h; y++) {
            distance[x][y] =
                exp( - sqr(fDistance[x][y] - fFocalPlanePos) / fDOFFactor  );
        }

#define depth(x,y) (distance[x][y])

    unsigned int wndSize = fHalfWndSize;

    //arrays filling
#define PUT(id, add) id[x][y] =             \
    - ((x > 0 && y > 0) ? id[x-1][y-1] : 0) \
    + ((x > 0) ? id[x-1][y] : 0)            \
    + ((y > 0) ? id[x][y-1] : 0) + add

#define GET(id) ( \
    id[ min(w-1, x+wndSize) ][ min(h-1, y+wndSize) ]                         \
    - ((y >= wndSize +1) ? id[ min(w-1, x+wndSize) ][ y-wndSize-1 ] : 0)     \
    - ((x >= wndSize +1) ? id[ x-wndSize-1 ][ min(h-1, y+wndSize) ] : 0)     \
    + ((x >= wndSize +1 && y >= wndSize +1) ? id[x-wndSize-1][y-wndSize-1] : 0) )

#define NUMEL(x,y) \
    (( min(w-1, (x+wndSize)) - max(0, (int) (x-wndSize)) +1 ) * \
     ( min(h-1, (y+wndSize)) - max(0, (int) (y-wndSize)) +1 ))

    //first pass: mean, variance, I*p, p
    for (unsigned int x = 0; x < w; x++)
        for (unsigned int y = 0; y < h; y++) {
            QRgb pix = image.pixel(x,y);
            PUT(sum, depth(x,y) );
            PUT(sec, sqr(depth(x,y)) );
            PUT(p_r, qRed(pix)   / 255.0);
            PUT(p_g, qGreen(pix) / 255.0);
            PUT(p_b, qBlue(pix)  / 255.0);
            PUT(Ip_r, depth(x,y) * qRed(pix)   / 255.0);
            PUT(Ip_g, depth(x,y) * qGreen(pix) / 255.0);
            PUT(Ip_b, depth(x,y) * qBlue(pix)  / 255.0);
        }

    //second pass: a, b
    for (unsigned int x = 0; x < w; x++)
        for (unsigned int y = 0; y < h; y++) {
            wndSize = (1 - depth(x,y)) * fHalfWndSize;
            unsigned int N = NUMEL(x,y);
            ftype mean = GET(sum) / N;
            ftype den = ( GET(sec) / N - sqr(mean) + fRegularization );
            ftype _a_r = ( GET(Ip_r) - mean * GET(p_r) ) / N / den;
            ftype _a_g = ( GET(Ip_g) - mean * GET(p_g) ) / N / den;
            ftype _a_b = ( GET(Ip_b) - mean * GET(p_b) ) / N / den;
            PUT(a_r, _a_r);
            PUT(a_g, _a_g);
            PUT(a_b, _a_b);
            PUT(b_r, GET(p_r)/ N - _a_r * mean);
            PUT(b_g, GET(p_g)/ N - _a_g * mean);
            PUT(b_b, GET(p_b)/ N - _a_b * mean);
        }

    //third pass: output image
    for (unsigned int x = 0; x < w; x++)
        for (unsigned int y = 0; y < h; y++)
        {
            //principal modification: window size is varying in fucntion of depth
            wndSize = (1 - depth(x,y)) * fHalfWndSize;
            unsigned int N = NUMEL(x,y);
            float
                r = (GET(a_r) * depth(x,y) + GET(b_r) )/ N * 255.f,
                g = (GET(a_g) * depth(x,y) + GET(b_g) )/ N * 255.f,
                b = (GET(a_b) * depth(x,y) + GET(b_b) )/ N * 255.f;
            QRgb color = qRgb(
                min(max(0.f, r), 255.f),
                min(max(0.f, g), 255.f),
                min(max(0.f, b), 255.f)
            );
            image.setPixel(x, y, color);
        }

    //memory disposing
    for (unsigned int x = 0; x < w; x++) {
        delete[] sum[x];
        delete[] sec[x];
        delete[] Ip_r[x];
        delete[] Ip_g[x];
        delete[] Ip_b[x];
        delete[] p_r[x];
        delete[] p_g[x];
        delete[] p_b[x];
        delete[] a_r[x];
        delete[] a_g[x];
        delete[] a_b[x];
        delete[] b_r[x];
        delete[] b_g[x];
        delete[] b_b[x];
        delete[] distance[x];
    }
    delete[] sum;
    delete[] sec;
    delete[] Ip_r;
    delete[] Ip_g;
    delete[] Ip_b;
    delete[] p_r;
    delete[] p_g;
    delete[] p_b;
    delete[] a_r;
    delete[] a_g;
    delete[] a_b;
    delete[] b_r;
    delete[] b_g;
    delete[] b_b;
    delete[] distance;
}


static const float DISTANCE_LIMIT = 100.f;

void GuidedFilter::adjustFocalPlane() {
    const float SEARCH_RANGE_MIN = 0.45;
    const float SEARCH_RANGE_MAX = 0.55;
    double sum = 0;
    unsigned int n = 0;
    for (unsigned int x = fImgWidth * SEARCH_RANGE_MIN; x < fImgWidth * SEARCH_RANGE_MAX; x++)
        for (unsigned int y = fImgHeight* SEARCH_RANGE_MIN; y < fImgHeight * SEARCH_RANGE_MAX; y++)
            if (fDistance[x][y] < DISTANCE_LIMIT)
            {
                sum += fDistance[x][y];
                n++;
            }
    if (n > 0)
        fFocalPlanePos = sum / n;
    else
        fFocalPlanePos = 1.0f;  //arbitraire
}


void GuidedFilter::resetDistances() {
    for (unsigned int x = 0; x < fImgWidth; x++)
        for (unsigned int y = 0; y < fImgHeight; y++)
            fDistance[x][y] = DISTANCE_LIMIT;
}
