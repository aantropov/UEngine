#pragma once
#include "UFrameBufferObject.h"
#include "..\Resources\UPostEffect.h"
#include "..\Resources\UTexture.h"
#include "ULighting.h"

using namespace std;
class UScene;

class URenderQueue
{
public:

    map<int, list<pair<mat4, UMesh*>>> data;
};

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
 
    static void RenderQueue(const URenderQueue& render_queue, const URenderPass type, const UCamera& camera);
    friend class UForwardLighting;
    friend class UDefferedLighting;

public:    

    void Render(UScene* scene);

    URenderManager();
    ~URenderManager(void);
};

