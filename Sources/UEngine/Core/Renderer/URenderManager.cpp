#include "URenderManager.h"
#include "URenderer.h"
#include "..\Basic\UScene.h"
#include "..\Utils\enum.h"
#include "..\Basic\ULight.h"
#include "..\UEngine.h"

URenderManager::URenderManager()
{
    if (atoi(UConfig::GetInstance()->GetParam("/xml/config/deffered_lighting/").c_str()) == 1)
        lighting = new UDefferedLighting();
    else
        lighting = new UForwardLighting();

    vsm_fbo.Initialize();
    post_effect_fbo.Initialize();
    post_post_effect_fbo.Initialize();

    postEffectSSAO = dynamic_cast<UPostEffect*>(UEngine::rf.Load("data\\PostEffects\\post_effect_ssao.xml", URESOURCE_POST_EFFECT));
    postEffectDOF = dynamic_cast<UPostEffect*>(UEngine::rf.Load("data\\PostEffects\\post_effect_dof.xml", URESOURCE_POST_EFFECT));
    postEffectRipple = dynamic_cast<UPostEffect*>(UEngine::rf.Load("data\\PostEffects\\post_effect_blur.xml", URESOURCE_POST_EFFECT));

    posteffectScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));

    posteffectScene->Create(URenderer::GetInstance()->GetWidth(), URenderer::GetInstance()->GetHeight(), UTEXTURE_COLOR);
    posteffectScene->name = "colorScene";

    postEffectRipple->AddTexture(lighting->resScene, 0);
    postEffectRipple->AddTexture(lighting->depthScene, 1);

    postEffectDOF->AddTexture(lighting->resScene, 0);
    postEffectDOF->AddTexture(lighting->depthScene, 1);

    postEffectSSAO->AddTexture(lighting->resScene, 2);
    postEffectSSAO->AddTexture(lighting->depthScene, 1);
    postEffectSSAO->AddTexture(lighting->normalScene, 3);

    depthTextureSize = atoi(UConfig::GetInstance()->GetParam("/xml/config/depth_texture_size/").c_str());
    depthShadowMap = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));

    depthShadowMap->Create(depthTextureSize, depthTextureSize, UTEXTURE_DEPTH, UTEXTURE_FILTER::UTEXTURE_FILTER_LINEAR, UTEXTURE_WRAP::UTEXTURE_WRAP_CLAMP_TO_EDGE);
    depthShadowMap->name = "depth";
}

URenderManager::~URenderManager(void)
{
    delete lighting;
}

void URenderManager::Render(UScene* scene)
{
    auto render = URenderer::GetInstance();

    UCamera previousCam = URenderer::GetInstance()->GetCurrentCamera();

    auto lights = scene->GetLights();
    auto light_params = render->GetCurrentScene()->light_params;
   
    render->BindFBO(&vsm_fbo);

    vsm_fbo.BindTexture(depthShadowMap, UFB_ATTACHMENT_DEPTH);
    glCullFace(GL_FRONT);
    glColorMask(GL_TRUE, GL_TRUE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_TRUE);
    glViewport(0, 0, depthTextureSize, depthTextureSize);

    glClearColor(1, 1, 1, 1);
    glClearDepth(1);

    //glEnable(GL_POLYGON_OFFSET_FILL);
    //glPolygonOffset(4, 4);

    for (unsigned int i = 0; i < light_params.count; i++)
    {
        if (!lights[light_params.light_index[i]]->IsShadowCaster())
            continue;

        auto vsmTextures = lights[light_params.light_index[i]]->GetDepthTextures();
        for (unsigned int j = 0; j < vsmTextures.size(); j++)
        {
            vsm_fbo.BindTexture(vsmTextures[j], UFB_ATTACHMENT_COLOR0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            scene->RenderQueue(URENDER_PASS_DEPTH_SHADOW, lights[light_params.light_index[i]]->GetCameras()[j]);
        }
    }
    
    render->UnbindFBO();

    auto helper = URendererHelper::GetInstance();

    for (unsigned int i = 0; i < light_params.count; i++)
    {
        if (!lights[light_params.light_index[i]]->IsShadowCaster())
            continue;

        auto vsmTextures = lights[light_params.light_index[i]]->GetDepthTextures();
        for (unsigned int j = 0; j < vsmTextures.size(); j++)
        {
            helper->GaussBlur(vsmTextures[j], 0.5f, vec2_x);
            helper->GaussBlur(vsmTextures[j], 0.5f, vec2_y);
        }
    }
    
    glClearColor(0, 0, 0, 1);
    //depthFbo.UnbindTexture(UFB_ATTACHMENT_COLOR0);
    //depthFbo.UnbindTexture(UFB_ATTACHMENT_DEPTH);
    //glDisable(GL_POLYGON_OFFSET_FILL);


    render->SetCurrentCamera(previousCam);

    OPENGL_CHECK_FOR_ERRORS();

    lighting->Render(scene, render->main_ñamera);
    //postEffectDOF->Render(URENDER_FORWARD);

    //postEffectSSAO->AddTexture(lights[light_params.light_index[0]]->GetDepthTextures()[0], 2);
    //postEffectSSAO->Render(URENDER_FORWARD);
    postEffectRipple->Render(URENDER_PASS_FORWARD);
}