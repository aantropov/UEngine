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

    depthFbo.Initialize();
    postEffectFbo.Initialize();
    postpostEffectFbo.Initialize();

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
    auto lightParams = render->GetCurrentScene()->lightParams;
   

    render->BindFBO(&depthFbo);

    depthFbo.BindTexture(depthShadowMap, UFB_ATTACHMENT_DEPTH);
    glCullFace(GL_FRONT);
    glColorMask(GL_TRUE, GL_TRUE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_TRUE);
    glViewport(0, 0, depthTextureSize, depthTextureSize);

    glClearColor(1000.0f, 1000.0f, 1.0f, .0f);
    //glClearDepth(1000.0f);

    //glEnable(GL_POLYGON_OFFSET_FILL);
    //glPolygonOffset(4, 4);

    for (unsigned int i = 0; i < lightParams.count; i++)
    {
        if (!lights[lightParams.lightIndex[i]]->castShadows)
            continue;

        auto depthTextures = lights[lightParams.lightIndex[i]]->GetDepthTextures();
        for (unsigned int j = 0; j < depthTextures.size(); j++)
        {
            depthFbo.BindTexture(depthTextures[j], UFB_ATTACHMENT_COLOR0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            scene->Render(URENDER_DEPTH, lights[lightParams.lightIndex[i]]->GetCameras()[j]);
        }
    }

    render->UnbindFBO();

    //depthFbo.UnbindTexture(UFB_ATTACHMENT_COLOR0);
    //depthFbo.UnbindTexture(UFB_ATTACHMENT_DEPTH);
    //glDisable(GL_POLYGON_OFFSET_FILL);


    render->SetCurrentCamera(previousCam);

    OPENGL_CHECK_FOR_ERRORS();

    lighting->Render(scene, render->mainCamera);
    //postEffectDOF->Render(URENDER_FORWARD);

    //postEffectSSAO->AddTexture(lights[lightParams.lightIndex[0]]->GetDepthTextures()[0], 2);
    postEffectSSAO->Render(URENDER_FORWARD);
    //postEffectRipple->Render(URENDER_FORWARD);
}