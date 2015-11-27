#ifndef PARAMETERHANDLER_H
#define PARAMETERHANDLER_H

#include <cmath>

class ParameterHandler
{    
private:
    int             m_scene;
    int             m_threadCount;
    bool            m_filter;
    bool            m_interactiveRender;

    bool            m_ambientOcclusion;

    bool            m_pathTracing;
    bool            m_rayTracing;
    unsigned int    m_maxRayDepth;
    unsigned int    m_pathTracingDiffuseRayCount;

    bool            m_antiAliasing;
    unsigned short  m_antiAliasingFactor;

    bool            m_shadows;
    bool            m_hardShadows;
    bool            m_softShadows;
    float           m_lightRadius;
    unsigned int    m_lightSamples;

    bool            m_kdTreeDone;

private:
    ParameterHandler ()
        :   m_scene(0),
            m_threadCount(2),
            m_filter(false),
            m_interactiveRender(false),
            m_ambientOcclusion ( false ),
            m_pathTracing ( false ),
            m_rayTracing ( true ),
            m_maxRayDepth ( 3 ),
            m_pathTracingDiffuseRayCount ( 5 ),
            m_antiAliasing ( true ),
            m_antiAliasingFactor ( 2 ),
            m_shadows ( true ),
            m_hardShadows ( false ),
            m_softShadows ( true ),
            m_lightRadius ( 0.5f ),
            m_lightSamples ( 20u ),
            m_kdTreeDone ( false )
    {}
    ~ParameterHandler ()
    {}

public:
    static inline ParameterHandler* Instance ()
    {
        static ParameterHandler _instance;
        return &_instance;
    }

    void SetScene (
        const int&             iScene
    );

    const int& GetScene () const;

    void SetThreadCount (
        const int&             iThread
    );

    const int& GetThreadCount () const;

    void SetFilter (
        const bool&             iFilter
    );
    const bool& GetFilter () const;

    void SetInteractiveRender (
       const bool&             interactive
    );
    const bool& GetInteractiveRender () const;

    void SetAo (
        const bool&             iAoFlag
    );
    const bool& GetAo () const;

    void SetPathTracing (
        const bool&             iPathTracingFlag
    );
    const bool& GetPathTracing () const;

    void SetMaxRayDepth (
        const unsigned int&     iMaxRayDepth
    );
    const unsigned int& GetMaxRayDepth () const;

    void SetPathTracingDiffuseRayCount (
        const unsigned int&     iPathTracingDiffuseRayCount
    );
    const unsigned int& GetPathTracingDiffuseRayCount () const;

    void SetRayTracing (
        const bool&             iRayTracingFlag
    );
    const bool& GetRayTracing () const;

    void SetAa (
        const bool&             iAaFlag
    );
    const bool& GetAa () const;

    void SetAaFactor (
        const unsigned short&     iAaFactor
    );
    const unsigned short &GetAaFactor() const;

    void SetShadows (
        const bool&             iShadowsFlag
    );
    const bool& GetShadows () const;

    void SetHardShadows (
        const bool&             iHardShadowsFlag
    );
    const bool& GetHardShadows () const;

    void SetSoftShadows (
        const bool&             iSoftShadowsFlag
    );
    const bool& GetSoftShadows () const;

    void SetLightRadius (
        const float&            iLightRadius
    );
    const float& GetLightRadius () const;

    void SetLightSamples (
        const unsigned int&     iLightSamples
    );
    const unsigned int& GetLightSamples () const;

    void SetKdTreeBuilt (
        const bool&             iKdTreeBuiltFlag
    );
    const bool& GetKdTreeBuilt () const;
};

#endif // PARAMETERHANDLER_H
