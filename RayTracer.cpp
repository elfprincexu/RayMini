// *********************************************************
// Ray Tracer Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2012 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "RayTracer.h"
#include "Ray.h"
#include "Scene.h"
#include <QProgressDialog>
#include <QCoreApplication>
#include <iostream>
#include <stdio.h>

#include "kd/KdTree.h"
#include "ParameterHandler.h"
#include "RadianceCalculator.h"
#include "GuidedFilter.h"
#include "Pbgi.h"
#include <omp.h>

using namespace kd;

#ifdef GetObject
#undef GetObject    //stupid Windows trick...
#endif

Vec3Df DoTracePath (
    const Scene&            iScene,
    const Ray&              iRay,
    const unsigned int&     iDepth,
    bool&                   oHasIntersection,
    Vec3Df&                 oIntersectionPoint
) {
    const ParameterHandler* params = ParameterHandler::Instance ();
    const RadianceCalculator* rc = RadianceCalculator::Instance ();
    const KdTree* kdTree = iScene.getKdTree ();
    KdIntersectionData intersection;

    Vec3Df color ( 0.0f, 0.0f, 0.0f );
    if (
        (
            oHasIntersection = kdTree->Intersect (
                iRay,
                intersection,
                0.00000000000000000005f
            )
        )
    ) {
        const Vec3Df& interPoint  = intersection.GetIntersectionPoint ();
        const Vec3Df& interNormal = intersection.GetIntersectionNormal ();
        const Material& interMaterial = intersection.GetObject () ->getMaterial ();
        oIntersectionPoint = interPoint;
        if (
            iDepth < params->GetMaxRayDepth ()
        ) {
            std::default_random_engine generator ( rand () );
            std::uniform_real_distribution<float> distributionTeta(0,2*M_PI);
            std::uniform_real_distribution<float> distributionPhi(0,M_PI/2);

            Vec3Df newDir = Vec3Df::polarToCartesian(
                Vec3Df (
                    1.0f,
                    distributionTeta ( generator ),
                    distributionPhi ( generator )
                )
            );
            
            float cosine;
            if (
                ( cosine = Vec3Df::dotProduct ( newDir, interNormal ) ) < 0
            ) {
                newDir *= -1.0f;
                cosine *= -1.0f;
            }            

            Ray reflectionRay ( interPoint, newDir );
            bool hasIntersection;
            Vec3Df refInter;
            Vec3Df reflectionColor = DoTracePath (
                iScene,
                reflectionRay,
                iDepth + 1,
                hasIntersection,
                refInter
            );
            if (
                ( hasIntersection )
            ) {
                color  += rc->Phong (
                    interPoint,
                    interNormal,
                    iRay.getOrigin (), 
                    refInter, 
                    reflectionColor,
                    interMaterial 
                );// * cosine / M_PI;
            }
        }
        color += rc->DirectLighting (
            iScene,
            iRay.getOrigin (),
            interPoint,
            interNormal,
            interMaterial
        );
    } else {
        color = Vec3Df ( 0.0f, 0.0f, 0.0f );
    }
    
    return color;
}

// Experimental!
Vec3Df TracePath (
    const Scene&        iScene,
    const Ray&          iRay,
    const Vec3Df&       iBackgroundColor
) {
    ParameterHandler* params = ParameterHandler::Instance ();
    bool hasIntersection = false;
    const unsigned int& rayCount = params->GetPathTracingDiffuseRayCount ();

    Vec3Df accumulator ( 0.0f, 0.0f, 0.0f );
    for (
        unsigned int i = 0;
        i < rayCount;
        i++
    ) {
        Vec3Df intersectionPoint;
        accumulator += DoTracePath (
            iScene,
            iRay,
            0, 
            hasIntersection,
            intersectionPoint
        );
        if (
                ( i == 0 )
            &&  ( !hasIntersection )
        ) {
            return iBackgroundColor;
        }
    }
    
    return accumulator / ( (float)rayCount );
}

Vec3Df DoTraceRay (
    const Scene&            iScene,
    const Ray&              iRay,
    const unsigned int&     iDepth,
    bool&                   oHasIntersection,
    KdIntersectionData&     oIntersection,
    const Vec3Df&           iBackgroundColor
) {
    const ParameterHandler* params = ParameterHandler::Instance ();
    const RadianceCalculator* rc = RadianceCalculator::Instance ();
    const KdTree* kdTree = iScene.getKdTree ();

    if (
        ( oHasIntersection = kdTree->Intersect (
                iRay,
                oIntersection,
                0.00000000000000000005f
            )
        )
    ) {
        const Vec3Df& interPoint  = oIntersection.GetIntersectionPoint ();
        const Vec3Df& interNormal = oIntersection.GetIntersectionNormal ();
        const Material& interMaterial = oIntersection.GetObject () ->getMaterial ();

        Vec3Df color ( 0.0f, 0.0f, 0.0f );
        if (
            iDepth < params->GetMaxRayDepth ()
        ) {
            const Vec3Df&   N               = interNormal;
            const Vec3Df&   Lm              = iRay.getDirection ();
            Vec3Df          reflectionDir   = Lm - 2 * N * Vec3Df::dotProduct(Lm, N);
            Ray reflectionRay ( interPoint, reflectionDir );

            bool hasIntersection = false;
            KdIntersectionData reflInter;
            Vec3Df reflectionColor = DoTraceRay (
                iScene,
                reflectionRay,
                iDepth + 1,
                hasIntersection,
                reflInter,
                iBackgroundColor
            );
            color += interMaterial.getColor () * interMaterial.getSpecular () * reflectionColor;
        }
        color += rc->DirectLighting (
            iScene,
            iRay.getOrigin (),
            interPoint,
            interNormal,
            interMaterial
        );
        
        return color;
    } 
    
    return iBackgroundColor;
}

Vec3Df TraceRay (
    const Scene&        iScene,
    const Ray&          iRay,
    const Vec3Df&       iBackgroundColor
) {
    ParameterHandler* params = ParameterHandler::Instance ();
    RadianceCalculator* rc = RadianceCalculator::Instance ();

    bool hasIntersection = false;
    KdIntersectionData intersectionData;
    Vec3Df color = DoTraceRay (
        iScene,
        iRay,
        0, 
        hasIntersection,
        intersectionData,
        iBackgroundColor
    );
    if (
        ( hasIntersection )
    ) {
        if (
            ( params->GetAo () )
        ) {
            const BoundingBox& bb = iScene.getBoundingBox ();

            float sceneDist = 0.05f * Vec3Df::distance (
                bb.getMin (),
                bb.getMax ()
            );
            float aoRatio = rc->AmbientOcclusion (
                20,
                sceneDist,
                intersectionData.GetIntersectionNormal (),
                intersectionData.GetIntersectionPoint (),
                iScene
            );
            color *= ( 1.0f - aoRatio );
        }
    }
    return color;
}


Vec3Df PathTracing(
    const Ray&              ray,            // incident ray
    const Scene*            scene,          // the scene
    const unsigned int&     depth=0         // Recursion level
) {
    const KdTree& kdTree = *(scene->getKdTree ());
    ParameterHandler* params = ParameterHandler::Instance ();
    RadianceCalculator* rc = RadianceCalculator::Instance ();

    KdIntersectionData intData;

    if (!kdTree.Intersect(ray, intData))
        return Vec3Df();

    const Object* obj = intData.GetObject ();
    Vec3Df point  = intData.GetIntersectionPoint ();
    Vec3Df normal = intData.GetIntersectionNormal ();
    normal.normalize();

    Vec3Df directPart = rc->DirectLighting (
        *scene,
        ray.getOrigin (),
        point,
        normal,
        obj->getMaterial ()
    );

    Vec3Df dir = ray.getDirection();
    dir.normalize();
  
    //diffuse component modelling
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distributionTeta(0,2*M_PI);
    std::uniform_real_distribution<float> distributionPhi(0,M_PI);
    generator.seed( rand() );

    Vec3Df diffusePart(0.f, 0.f, 0.f);
    if (
            ( obj->getMaterial().getDiffuse() > 0 )
        &&  ( depth == 0 )
    ) {
        for (
            unsigned int rayCounter = 0;
            rayCounter < params->GetPathTracingDiffuseRayCount();
            rayCounter++
        ) {
            Vec3Df newDir = Vec3Df::polarToCartesian( Vec3Df(1, distributionTeta(generator), distributionPhi(generator)) );
            
            if ( Vec3Df::dotProduct(newDir, normal) < 0 ) {
                newDir = -newDir;
            }
            Ray newRay(intData.GetIntersectionPoint(), newDir);
            diffusePart +=
                PathTracing(newRay, scene, depth+1)
                * Vec3Df::dotProduct(normal, newDir);
        }
        diffusePart = diffusePart 
                    * obj->getMaterial().getDiffuse() 
                    * obj->getMaterial().getColor() 
                    / params->GetPathTracingDiffuseRayCount() 
                    * (2 * M_PI);
    }

    //specular component modelling
    Vec3Df specularPart(0.f, 0.f, 0.f);
    if (obj->getMaterial().getSpecular() > 0  &&  depth < params->GetMaxRayDepth() ) {
        Ray newRay(intData.GetIntersectionPoint(), -2*Vec3Df::projectOntoVector(dir, normal) + dir);
        float cos_ = Vec3Df::dotProduct(-dir, normal);
        if (cos_ > 0) {
            specularPart =
                obj->getMaterial().getSpecular()
                * PathTracing(newRay, scene, depth+1)
                * obj->getMaterial().getColor();
                //* pow(cos_, obj->getMaterial().getShininess() );
        }
    }

    return (directPart + diffusePart + specularPart);
}

static RayTracer * instance = NULL;

RayTracer * RayTracer::getInstance () {
    if (instance == NULL)
        instance = new RayTracer ();
    return instance;
}

void RayTracer::destroyInstance () {
    if (instance != NULL) {
        delete instance;
        instance = NULL;
    }
}

// POINT D'ENTREE DU PROJET.
// Le code suivant ray trace uniquement la boite englobante de la scene.
// Il faut remplacer ce code par une veritable raytracer
QImage RayTracer::render (
    const Vec3Df & camPos,
    const Vec3Df & direction,
    const Vec3Df & upVector,
    const Vec3Df & rightVector,
    float fieldOfView,
    float aspectRatio,
    unsigned int screenWidth,
    unsigned int screenHeight)
{
    Scene * scene = Scene::getInstance ();
    const std::vector<Light>& lights = scene->getLights();

    QProgressDialog* progressDialog = NULL;
    if (!fInterRenderer.isEnabled())
        progressDialog = new QProgressDialog("Raytracing...", "Cancel", 0, 100);

    ParameterHandler* params = ParameterHandler::Instance ();
    if ( !params->GetKdTreeBuilt () ) {
        QProgressDialog* ktDialog = NULL;
        if (!fInterRenderer.isEnabled()) {
            ktDialog = new QProgressDialog ("Building KD Tree...", "Cancel", 0, 100);
            ktDialog->show ();
        }

        scene->buildKdTree ();
        if (ktDialog) {
            ktDialog->setValue ( 100 );
            ktDialog->close();
            delete ktDialog;
        }
        params->SetKdTreeBuilt ( true );
    }
    const KdTree* kt = scene->getKdTree ();

    if (progressDialog)
        progressDialog->show ();
    
    Vec3Df ambientColor ( 0, 0, 0 );
    for ( unsigned int l = 0; l < lights.size(); l++ ) {
        const Light&    light = lights[l];
        const Vec3Df&   lightColor = light.getColor();
        const float&    lightIntensity = light.getIntensity();

        ambientColor += lightColor * lightIntensity;
    }
    ambientColor /= lights.size();

    //Pbgi::Instance ()->IlluminatePointCloud ( *scene );

    //initializing image set
    const unsigned short& AAFactor = ( params->GetAa() ) ? params->GetAaFactor() : 1;
    unsigned int RaysParPixel = AAFactor*AAFactor;
    if (fInterRenderer.isEnabled())
        RaysParPixel = 1;       //for interactive rendering anti-aliasing is just a question of time

    std::vector<QImage*> images;
    images.resize(RaysParPixel);
    for (unsigned int imgCounter = 0; imgCounter < RaysParPixel; imgCounter++) {
        images[imgCounter] = new QImage(QSize(screenWidth, screenHeight), QImage::Format_RGB888);
    }

    //let's go
    int progress = 0;
    for (unsigned int imgCounter = 0; imgCounter < RaysParPixel; imgCounter++) {
        float OffsetX = 1.0f * (imgCounter % AAFactor) / AAFactor;
        float OffsetY = 1.0f * ((unsigned int) (imgCounter / AAFactor)) / AAFactor;
        if (fInterRenderer.isEnabled()) {
            OffsetX += fInterRenderer.getXOffset();
            OffsetY += fInterRenderer.getYOffset();
        }

        GuidedFilter filter(screenWidth, screenHeight);

        const unsigned int& threadCount = ( params->GetThreadCount() ) ? params->GetThreadCount() : 2;
        
        #pragma omp parallel for
        for (unsigned int threadIdx = 0; threadIdx < threadCount; threadIdx++) {
            for (unsigned int i = threadIdx; i < screenWidth; i += threadCount )
                if (!fInterRenderer.isEnabled()  ||  !fInterRenderer.wasCancelled()) {
                    #pragma omp critical
                    {
                        progress++;
                    }

                    if (threadIdx == 0 && progressDialog) /* master thread */
                    {
                        progressDialog->setValue ((100*progress)/(RaysParPixel*screenWidth));
                    }

                    for ( unsigned int j = 0; j < screenHeight; j++ ) {
                        float tanX = tan (fieldOfView)*aspectRatio;
                        float tanY = tan (fieldOfView);

                        Vec3Df radiance ( backgroundColor );
                        Vec3Df stepX = (float (i) + OffsetX - screenWidth / 2.f) / screenWidth * tanX * rightVector;
                        Vec3Df stepY = (float (j) + OffsetY - screenHeight / 2.f) / screenHeight * tanY * upVector;
                        Vec3Df step = stepX + stepY;
                        Vec3Df dir = direction + step;
                        dir.normalize ();

                        Ray ray ( camPos, dir );
                        KdIntersectionData intersectionData;

                        if ( params->GetPathTracing () ) {
                            //PATH TRACING
                            if ( kt->Intersect ( Ray ( camPos, dir ), intersectionData ) ) {
                                if (!fInterRenderer.isEnabled())
                                    filter.setDistance(
                                        i, j,
                                        Vec3Df::distance(intersectionData.GetIntersectionPoint(), camPos)
                                    );
                                //radiance = 255.f * TracePath (*scene, ray, backgroundColor/255.f);
                                radiance = 255.f * PathTracing (
                                    ray,
                                    scene
                                );
                            }
                        } else if ( params->GetRayTracing () ) {
                            //DIRECT LIGHTNING
                            if ( kt->Intersect ( Ray ( camPos, dir ), intersectionData ) ) {
                                if (!fInterRenderer.isEnabled())
                                    filter.setDistance(
                                        i, j,
                                        Vec3Df::distance(intersectionData.GetIntersectionPoint(), camPos)
                                    );
                                radiance = 255.f * TraceRay (
                                    *scene,
                                    ray,
                                    backgroundColor/255.f
                                );
                            }
                        } else {
                        }

                        QRgb color = qRgb (
                                min(255.0f, radiance[0]),
                                min(255.0f, radiance[1]),
                                min(255.0f, radiance[2])
                                );

                        /*for (unsigned int xx = i; xx < min(screenWidth, i + CeilW); xx ++)
                            for (unsigned int yy = j; yy < min(screenHeight, j + CeilH); yy ++)*/
                        images[imgCounter] -> setPixel (i, j, color);
                    }

                    /*if (threadIdx == 0 && !fInteractive) {
                        QCoreApplication::processEvents();
                        if (progressDialog.wasCanceled())
                            break;
                    }*/
                }
            }

            if (!fInterRenderer.isEnabled())
                filter.adjustFocalPlane();

            if ( params->GetFilter () && !fInterRenderer.isEnabled() ) {
                filter.apply(*images[imgCounter]);
            }
        }
    

    if (progressDialog)
        progressDialog->setValue (100);

    QImage image = *images[0];
    if (RaysParPixel > 1 && (!fInterRenderer.isEnabled() || !fInterRenderer.wasCancelled())) {
        for ( unsigned int i = 0; i < screenWidth; i++ ) {
            for ( unsigned int j = 0; j < screenHeight; j++ ) {
                unsigned int r = 0, g = 0, b = 0;
                for (unsigned int imgCounter = 1; imgCounter < RaysParPixel; imgCounter++) {
                    QRgb pix = images[imgCounter]->pixel( QPoint(i,j) );
                    r += qRed(pix);
                    g += qGreen(pix);
                    b += qBlue(pix);
                }
                QRgb pix = image.pixel( QPoint(i,j) );
                r = min(255u, (r + qRed(pix)  ) / RaysParPixel);
                g = min(255u, (g + qGreen(pix)) / RaysParPixel);
                b = min(255u, (b + qBlue(pix) ) / RaysParPixel);
                image.setPixel(i,j, qRgb(r,g,b));
            }
        }
    }

    for (unsigned int imgCounter = 0; imgCounter < RaysParPixel; imgCounter++) {
        delete images[imgCounter];
    }

    if (progressDialog)
        delete progressDialog;

    return image;
}

/*
//Backup of pre-kdtree radiance calculation code. Do not delete nor mess with it.
Vec3Df intersectionPoint;
Vec3Df intersectionNormal;
float smallestIntersectionDistance = 1000000.f;
Vec3Df c (backgroundColor);
for (int k = 0; k < scene->getObjects().size (); k++) {
    const Object & o = scene->getObjects()[k];
    Ray ray (camPos-o.getTrans (), dir);

    //  Verify if Bound box has an intersection,
    //   if no intersection with box, there is no intersection with object
    bool hasIntersection = ray.intersect (o.getBoundingBox (),
                                          intersectionPoint);
    if (hasIntersection) {
        std::vector<Triangle> triangles = o.getMesh().getTriangles ();

        for (int tri = 0; tri < triangles.size(); tri++) {
            const Vertex& v0 = o.getMesh().getVertices()[triangles[tri].getVertex(0)];
            const Vertex& v1 = o.getMesh().getVertices()[triangles[tri].getVertex(1)];
            const Vertex& v2 = o.getMesh().getVertices()[triangles[tri].getVertex(2)];

            float t,u,v;
            bool intersects = ray.intersect(v0,v1,v2,t,u,v,false);
            intersectionNormal = (1 - u - v) * v0.getNormal()
                               + (    u    ) * v1.getNormal()
                               + (    v    ) * v2.getNormal();
            intersectionNormal.normalize();

            if (intersects) {
                if ( ( (t - smallestIntersectionDistance) < -0.0001) &&
                    Vec3Df::dotProduct(intersectionNormal, dir) < 0) {
                    smallestIntersectionDistance = t;
                    intersectionPoint = camPos + t * dir;

                    const Material& mat = o.getMaterial();
                    Vec3Df view = camPos - intersectionPoint;
                    view.normalize();

                    radiance = calculateRadiance (
                        mat,
                        lights,
                        ambientColor,
                        intersectionPoint,
                        intersectionNormal,
                        camPos
                    );
                }
            }
        }
    }
}
radiance =  radiance (1 - aoRatio);
*/
