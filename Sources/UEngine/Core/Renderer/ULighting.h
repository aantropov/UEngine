#pragma once
#include "UFrameBufferObject.h"
#include "..\Resources\UPostEffect.h"
#include "..\Resources\UTexture.h"

class UScene;
class UCamera;

class ULighting
{
public:

    UTexture* resScene;
    UTexture* depthScene;
    UTexture* normalScene;

    UFrameBufferObject fb;
    UFrameBufferObject postfb;
    
    virtual UTexture *Render(UScene *scene, UCamera camera) = 0;
};

class UForwardLighting : public ULighting
{
public:

    virtual UTexture *Render(UScene *scene, UCamera camera);
    UForwardLighting();
};

class UDefferedLighting : public ULighting
{
public:
    
    UTexture* colorScene;
    UTexture* diffuseScene;
    UTexture* ambientScene;
    UTexture* specularScene;
    UTexture* positionScene;    

    UTexture* resSceneA;
    UTexture* resSceneB;

    UPostEffect *lighting;
    UShaderProgram *objectShaderProgram;

    virtual UTexture *Render(UScene *scene, UCamera camera);
    UDefferedLighting();
};