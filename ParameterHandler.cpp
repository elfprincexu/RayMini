#include "ParameterHandler.h"

void ParameterHandler::SetScene (
    const int&             iScene
) {
    m_scene = iScene;
    m_kdTreeDone = false;
}


const int& ParameterHandler::GetScene () const
{
    return m_scene;
    
}

void ParameterHandler::SetThreadCount (
    const int&             iThread
) {
    m_threadCount = iThread;
}


const int& ParameterHandler::GetThreadCount () const
{
    return m_threadCount;
    
}

void ParameterHandler::SetAo (
    const bool&             iAoFlag
) {
    m_ambientOcclusion = iAoFlag;
}
const bool& ParameterHandler::GetAo () const
{
    return m_ambientOcclusion;
}

void ParameterHandler::SetFilter (
    const bool&             iFilter
) {
    m_filter = iFilter;
}
const bool& ParameterHandler::GetFilter () const
{
    return m_filter;
}

void ParameterHandler::SetInteractiveRender (
    const bool&             interactive
) {
    m_interactiveRender = interactive;
}
const bool& ParameterHandler::GetInteractiveRender () const
{
    return m_interactiveRender;
}

void ParameterHandler::SetPathTracing (
    const bool&             iPathTracingFlag
) {
    m_pathTracing = iPathTracingFlag;
}
const bool& ParameterHandler::GetPathTracing () const
{
    return m_pathTracing;
}

void ParameterHandler::SetMaxRayDepth (
    const unsigned int&     iMaxRayDepth
) {
    m_maxRayDepth = iMaxRayDepth;
}
const unsigned int& ParameterHandler::GetMaxRayDepth () const
{
    return m_maxRayDepth;
}

void ParameterHandler::SetPathTracingDiffuseRayCount (
    const unsigned int&     iPathTracingDiffuseRayCount
) {
    m_pathTracingDiffuseRayCount = iPathTracingDiffuseRayCount;
}
const unsigned int& ParameterHandler::GetPathTracingDiffuseRayCount () const
{
    return m_pathTracingDiffuseRayCount;
}

void ParameterHandler::SetRayTracing (
    const bool&             iRayTracingFlag
) {
    m_rayTracing = iRayTracingFlag;
}
const bool& ParameterHandler::GetRayTracing () const
{
    return m_rayTracing;
}

void ParameterHandler::SetAa (
    const bool&              iAaFlag
) {
    m_antiAliasing = iAaFlag;
}
const bool& ParameterHandler::GetAa () const
{
    return m_antiAliasing;
}

void ParameterHandler::SetAaFactor (
    const unsigned short&     iAaFactor
) {
    m_antiAliasingFactor = iAaFactor;
}
const unsigned short& ParameterHandler::GetAaFactor () const
{
    return m_antiAliasingFactor;
}

void ParameterHandler::SetShadows (
    const bool&             iShadowsFlag
) {
    m_shadows = iShadowsFlag;
}
const bool& ParameterHandler::GetShadows () const
{
    return m_shadows;
}

void ParameterHandler::SetHardShadows (
    const bool&             iHardShadowsFlag
) {
    m_hardShadows = iHardShadowsFlag;
}
const bool& ParameterHandler::GetHardShadows () const
{
    return m_hardShadows;
}

void ParameterHandler::SetSoftShadows (
    const bool&             iSoftShadowsFlag
) {
    m_softShadows = iSoftShadowsFlag;
}
const bool& ParameterHandler::GetSoftShadows () const
{
    return m_softShadows;
}

void ParameterHandler::SetLightRadius (
    const float&            iLightRadius
) {
    m_lightRadius = iLightRadius;
}
const float& ParameterHandler::GetLightRadius () const
{
    return m_lightRadius;
}

void ParameterHandler::SetLightSamples (
    const unsigned int&     iLightSamples
) {
    m_lightSamples = iLightSamples;
}
const unsigned int& ParameterHandler::GetLightSamples () const
{
    return m_lightSamples;
}

void ParameterHandler::SetKdTreeBuilt (
    const bool&             iKdTreeBuiltFlag
) {
    m_kdTreeDone = iKdTreeBuiltFlag;
}
const bool& ParameterHandler::GetKdTreeBuilt () const
{
    return m_kdTreeDone;
}
