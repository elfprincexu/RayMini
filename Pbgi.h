#ifndef _PBGI_H_
#define _PBGI_H_

class Pbgi {
private:
//    OcTree*      m_ocTree;

    Pbgi() {}
public:
    ~Pbgi ()
    {
//        if (
//            m_ocTree
//        ) {
//            delete m_ocTree;
//            m_ocTree = 0x0;
//        }
    }

    static Pbgi* Instance ()
    {
        static Pbgi _instance;
        return &_instance;
    }
    
//    void BuildOcTree (
//        const std::vector< Surfel* >
//    )
//    {} 
        
    //QImage render () const { return QImage(); }

    void IlluminatePointCloud (
        Scene&    iScene
    ) {
        const RadianceCalculator* rc = RadianceCalculator::Instance ();
        std::vector< Surfel* >& pointCloud = iScene.GetPointCloud ();

        for (
            unsigned int surfel = 0;
            surfel < pointCloud.size ();
            surfel++
        ) {
            const Vec3Df& surfelPos = pointCloud[surfel]->GetPosition ();
            const Vec3Df& surfelNormal = pointCloud[surfel]->GetNormal ();
            const Material& surfelMaterial = pointCloud[surfel]->GetMaterial ();

            Vec3Df surfelColor = rc->DirectLighting (
                iScene,
                surfelPos,
                surfelPos,
                surfelNormal,
                surfelMaterial
            );
                
            //Vec3Df color ( 0.0f, 0.0f, 0.0f );
            //for (
            //    std::vector< Light >::const_iterator light = sceneLights.begin();
            //    light != sceneLights.end();
            //    light++
            //) {
            //    

            //    float v = 1.0f;
            //    if ( params->GetShadows () ) {
            //        if ( params->GetSoftShadows () ) {
            //            std::vector< Vec3Df > lightSamples;

            //            light->getSamples (
            //                params->GetLightRadius (),
            //                params->GetLightSamples (),
            //                Vec3Df ( 0.0f, -1.0f, 0.0f ),//iInterPoint - light->getPos (),
            //                lightSamples
            //            );

            //            v = CalculateVisibility (
            //                iScene,
            //                surfelPos,
            //                lightSamples
            //            );
            //        } else {
            //            if (
            //                !IsVisible (
            //                    iScene,
            //                    surfelPos,
            //                    light->getPos ()
            //                )
            //            ) {
            //                v = 0.0f;
            //            }
            //        }
            //    }

            //    color += v * Phong (
            //        surfelPos,
            //        surfelNormal,
            //        surfelPos,
            //        light->getPos (),
            //        light->getColor () * light->getIntensity (),
            //        surfelMaterial
            //    );
            //}
            pointCloud[surfel]->SetColor ( surfelColor );
        }
    }
};

#endif // _PBGI_H_
