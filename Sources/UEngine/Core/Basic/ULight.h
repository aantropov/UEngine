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

class ULight :    public UGameObject
{
    
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 attenuation;

    vec3 spotDirection;
    float spotCosCutoff;
    float spotExponent;

    UModel* model;
    
    std::vector<UTexture*> depthTextures;
    std::vector<UCamera> cameras;

public:
    
    bool castShadows;

    std::vector<UTexture*> UENGINE_DECLSPEC GetDepthTextures();
    std::vector<UCamera> UENGINE_DECLSPEC GetCameras();

    vec4 UENGINE_DECLSPEC GetAmbient(){ return ambient; }
    vec4 UENGINE_DECLSPEC GetDiffuse(){ return diffuse; }
    vec4 UENGINE_DECLSPEC GetSpecular(){ return specular; }
    vec3 UENGINE_DECLSPEC GetAttenuation(){ return attenuation; }

    vec3 UENGINE_DECLSPEC GetSpotDirection(){ return spotDirection; }
    float UENGINE_DECLSPEC GetSpotCosCutoff(){ return spotCosCutoff; }
    float UENGINE_DECLSPEC GetSpotExponent(){ return spotExponent; }

    void UENGINE_DECLSPEC SetAmbient(vec4 v){ ambient = v; }
    void UENGINE_DECLSPEC SetDiffuse(vec4 v){ diffuse = v; }
    void UENGINE_DECLSPEC SetSpecular(vec4 v){ specular = v; }
    void UENGINE_DECLSPEC SetAttenuation(vec3 v){ attenuation = v; }

    void UENGINE_DECLSPEC SetSpotDirection(vec4 v){ spotDirection = v; }
    void UENGINE_DECLSPEC SetSpotCosCutoff(float degrees){ spotCosCutoff = cosf(degrees*math_radians); }
    void UENGINE_DECLSPEC SetSpotExponent(float v){ spotExponent = v; }

    virtual UENGINE_DECLSPEC mat4 GetLightTransform();
    virtual UENGINE_DECLSPEC void SetLightTransform(string light);
    virtual UENGINE_DECLSPEC void SetShadowTexture(unsigned int location, int i);
    
    // Deprecated!
    virtual void UENGINE_DECLSPEC SetShaderParameters(int numberOfLight);

    virtual void UENGINE_DECLSPEC Update(double delta);

    void UENGINE_DECLSPEC InitModel(UResourceFactory* rf);
    
    UENGINE_DECLSPEC ULight();
    UENGINE_DECLSPEC ULight(UResourceFactory* rf,  vec4 pos);
    UENGINE_DECLSPEC ULight(UResourceFactory* rf,  vec4 pos, bool _castShadows);
    UENGINE_DECLSPEC ~ULight(void);
};

