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

    map<UBlendMode, map<int, list<pair<mat4, UMesh*>>>> data;
};

class URenderManager
{
    UFrameBufferObject vsm_fbo;

    const int bloom_quality = 2;
    UPostEffect* bloom;
    vector<UTexture*> bloom_textures;
    UTexture* accum_bloom;

    UPostEffect* ripple;
    UPostEffect* ssao;
    UPostEffect* depth_of_field;
        
    UTexture* color;
    UTexture* depth;
    UTexture* normal;

    ULighting *opaque_lighting;
    ULighting *translucent_lighting;

    UTexture* depthShadowMap;
    int depthTextureSize;

    void RenderShadowMaps();

    static void RenderQueue(const URenderQueue& render_queue, const UBlendMode blend_mode, const URenderPass type, const UCamera& camera);

    friend class UForwardLightingOpaque;
    friend class UDefferedLightingOpaque;
    friend class UForwardLightingTranslucent;

public:

    void Render(UScene* scene);

    URenderManager();
    ~URenderManager(void);
};

