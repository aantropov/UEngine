#pragma once
#include "UFrameBufferObject.h"
#include "..\Resources\UPostEffect.h"
#include "..\Resources\UTexture.h"
#include "ULighting.h"

class UScene;

class URenderManager
{
    UFrameBufferObject vsm_fbo;
    UFrameBufferObject post_effect_fbo;
    UFrameBufferObject post_post_effect_fbo;
    
    UPostEffect* postEffectRipple;
    UPostEffect* postEffectSSAO;
    UPostEffect* postEffectDOF;
    
    UTexture* colorScene;
    UTexture* depthScene;
    UTexture* normalScene;
    UTexture* posteffectScene;

    ULighting *lighting;
    
    UTexture* depthShadowMap;
    int depthTextureSize;

public:

    void Render(UScene* scene);

    URenderManager();
    ~URenderManager(void);
};

