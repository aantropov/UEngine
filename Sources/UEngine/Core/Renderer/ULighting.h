#pragma once
#include "UFrameBufferObject.h"
#include "..\Resources\UPostEffect.h"
#include "..\Resources\UTexture.h"

class UScene;
class UCamera;
class URenderQueue;

class ULighting
{
public:

    UTexture* resScene;
    UTexture* depthScene;
    UTexture* normalScene;

    UFrameBufferObject fb;
    UFrameBufferObject postfb;

    virtual UTexture *Render(const UScene *scene, const UCamera camera, const URenderQueue& render_queue) = 0;

    friend class URenderManager;
};

class UForwardLighting : public ULighting
{
public:

    virtual  UTexture *Render(const UScene *scene, const UCamera camera, const URenderQueue& render_queue);
    UForwardLighting();
};

class UDefferedLighting : public ULighting
{
public:

    UTexture* colorScene;
    UTexture* diffuseScene;
    //UTexture* ambientScene;
    UTexture* specularScene;
    UTexture* positionScene;

    UTexture* resSceneA;
    UTexture* resSceneB;

    UPostEffect *lighting;
    UShaderProgram *objectShaderProgram;

    virtual  UTexture *Render(const UScene *scene, const UCamera camera, const URenderQueue& render_queue);
    UDefferedLighting();
};