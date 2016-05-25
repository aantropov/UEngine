#pragma once
#include "unode.h"
#include "..\Renderer\urenderer.h"
#include "UIRenderable.h"
#include "..\Resources\UTexture.h"
#include "UCamera.h"
#include "..\GameMechanics\UGameMechanics.h"

class UResourceFactory;
class UModel;
class UShaderProgram;

class ULight : public UGameObject
{
    ULIGHT_TYPE type;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 attenuation;

    vec3 spotDirection;
    float spotCosHalfAngle;
    float spotAngle;
    float spotExponent;
    float shadowDistanceMax;
    float shadowDistanceMin;

    bool castShadows;

    UModel* model;

    std::vector<UTexture*> depthTextures;
    std::vector<UCamera> cameras;

    void UpdateCamera();
    void UpdateDepthTextures();

public:

    std::vector<UTexture*> UENGINE_DECLSPEC GetDepthTextures();
    std::vector<UCamera> UENGINE_DECLSPEC GetCameras();

    ULIGHT_TYPE UENGINE_DECLSPEC GetType() const { return type; }
    vec4 UENGINE_DECLSPEC GetAmbient() const { return ambient; }
    vec4 UENGINE_DECLSPEC GetDiffuse() const { return diffuse; }
    vec4 UENGINE_DECLSPEC GetSpecular() const { return specular; }
    vec3 UENGINE_DECLSPEC GetAttenuation() const { return attenuation; }
    bool UENGINE_DECLSPEC IsShadowCaster() const { return castShadows; }

    vec3 UENGINE_DECLSPEC GetSpotDirection() const { return spotDirection; }
    float UENGINE_DECLSPEC GetSpotCosCutoff() const { return spotCosHalfAngle; }
    float UENGINE_DECLSPEC GetSpotExponent() const { return spotExponent; }

    float UENGINE_DECLSPEC GetShadowDistanceMax() const { return shadowDistanceMax; }
    float UENGINE_DECLSPEC GetShadowDistanceMin() const { return shadowDistanceMin; }

    void UENGINE_DECLSPEC IsShadowCaster(bool cast) { castShadows = cast; UpdateDepthTextures(); }
    void UENGINE_DECLSPEC SetType(ULIGHT_TYPE t) { type = t; }
    void UENGINE_DECLSPEC SetAmbient(vec4 v) { ambient = v; }
    void UENGINE_DECLSPEC SetDiffuse(vec4 v) { diffuse = v; }
    void UENGINE_DECLSPEC SetSpecular(vec4 v) { specular = v; }
    void UENGINE_DECLSPEC SetAttenuation(vec3 v) { attenuation = v; }

    void UENGINE_DECLSPEC SetSpotDirection(vec4 v) { spotDirection = v; }
    void UENGINE_DECLSPEC SetSpotCosCutoff(float degrees) { spotAngle = degrees;  spotCosHalfAngle = cosf(spotAngle * 0.5f * math_radians); }
    void UENGINE_DECLSPEC SetSpotExponent(float v) { spotExponent = v; }

    virtual UENGINE_DECLSPEC mat4 GetLightTransform();
    virtual UENGINE_DECLSPEC void SetLightTransform(string light);
    virtual UENGINE_DECLSPEC void SetShadowTexture(unsigned int location, int i);

    // Deprecated!
    virtual void UENGINE_DECLSPEC SetShaderParameters(int numberOfLight);

    virtual void UENGINE_DECLSPEC Update(double delta);

    void UENGINE_DECLSPEC InitModel(UResourceFactory* rf);

    UENGINE_DECLSPEC ULight();
    UENGINE_DECLSPEC ULight(UResourceFactory* rf, vec4 pos);
    UENGINE_DECLSPEC ULight(UResourceFactory* rf, vec4 pos, bool _castShadows);
    UENGINE_DECLSPEC ~ULight(void);
};