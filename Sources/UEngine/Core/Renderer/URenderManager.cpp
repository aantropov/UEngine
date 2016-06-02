#include "URenderManager.h"
#include "URenderer.h"
#include "..\Basic\UScene.h"
#include "..\Utils\enum.h"
#include "..\Basic\ULight.h"
#include "..\UEngine.h"
#include <vector>
#include <map>

URenderManager::URenderManager()
{
    if (atoi(UConfig::GetInstance()->GetParam("/xml/config/deffered_lighting/").c_str()) == 1)
        opaque_lighting = new UDefferedLightingOpaque();
    else
        opaque_lighting = new UForwardLightingOpaque();

    translucent_lighting = new UForwardLightingTranslucent(opaque_lighting->depth);

    vsm_fbo.Initialize();

    accum_bloom = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
    accum_bloom->name = "bluredScene";
    accum_bloom->SetMipMap(false);
    accum_bloom->Create(pow(2, bloom_quality - 1 + 4), pow(2, bloom_quality - 1 + 4), UTextureFormat::RGBA);

    for (int i = bloom_quality - 1; i >= 0; i--)
    {
        int quality = pow(2, i + 4);
        auto texture = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
        texture->name = "colorScene";
        texture->SetMipMap(false);
        texture->Create(quality, quality, UTextureFormat::RGBA);
        bloom_textures.push_back(texture);
    }

    bloom = dynamic_cast<UPostEffect*>(UEngine::rf.Load("data\\PostEffects\\post_effect_bloom.xml", UResourceType::PostEffect));
    ssao = dynamic_cast<UPostEffect*>(UEngine::rf.Load("data\\PostEffects\\post_effect_ssao.xml", UResourceType::PostEffect));
    depth_of_field = dynamic_cast<UPostEffect*>(UEngine::rf.Load("data\\PostEffects\\post_effect_dof.xml", UResourceType::PostEffect));
    ripple = dynamic_cast<UPostEffect*>(UEngine::rf.Load("data\\PostEffects\\post_effect_blur.xml", UResourceType::PostEffect));

    bloom->AddTexture(opaque_lighting->color, 0);
    ripple->AddTexture(opaque_lighting->color, 0);
    ripple->AddTexture(opaque_lighting->depth, 1);

    depth_of_field->AddTexture(opaque_lighting->color, 0);
    depth_of_field->AddTexture(opaque_lighting->depth, 1);

    ssao->AddTexture(opaque_lighting->color, 2);
    ssao->AddTexture(opaque_lighting->depth, 1);
    ssao->AddTexture(opaque_lighting->normal, 3);

    depthTextureSize = atoi(UConfig::GetInstance()->GetParam("/xml/config/depth_texture_size/").c_str());
    depthShadowMap = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
    depthShadowMap->Create(depthTextureSize, depthTextureSize, UTextureFormat::Depth32F, UTextureFiltration::Linear, UTextureWrapMode::ClampToEdge);
    depthShadowMap->name = "depth";
}

URenderManager::~URenderManager(void)
{
    delete opaque_lighting;
    delete translucent_lighting;
}

void URenderManager::Render(UScene* scene)
{
    auto render = URenderer::GetInstance();
    auto render_helper = URendererHelper::GetInstance();

    URenderQueue render_queue;
    scene->PrepareRenderQueue(render_queue);

    RenderShadowMaps();

    opaque_lighting->Render(scene, render->main_ñamera, render_queue);
    //translucent_lighting->Render(scene, render->main_ñamera, render_queue);

    //bloom
    render_helper->CopyTexture(opaque_lighting->color, bloom_textures[0]);
    for (int i = 1; i < bloom_quality; i++)
        render_helper->CopyTexture(bloom_textures[i - 1], bloom_textures[i]);

    render_helper->MixTextures(bloom_textures[0], 0.5f, bloom_textures[1], 0.5f, accum_bloom);

    render_helper->GaussBlur(accum_bloom, 1.0, vec2_x);
    render_helper->GaussBlur(accum_bloom, 1.0, vec2_y);

    //ripple->AddTexture(lights[light_params.light_index[1]]->GetDepthTextures()[0], 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, opaque_lighting->color->GetWidth(), opaque_lighting->color->GetHeight());

    accum_bloom->name = "bluredScene";
    bloom->AddTexture(accum_bloom, 1);
    bloom->Render(URenderPass::Forward);
}

void URenderManager::RenderShadowMaps()
{
    auto render = URenderer::GetInstance();
    auto scene = render->GetCurrentScene();
    auto lights = scene->GetLights();
    auto light_params = scene->light_params;
    auto previous_camera = URenderer::GetInstance()->GetCurrentCamera();

    render->BindFBO(&vsm_fbo);

    vsm_fbo.BindTexture(depthShadowMap, UFramebufferAttachment::Depth);
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
            auto camera = lights[light_params.light_index[i]]->GetCameras()[j];
            camera.UpdateFrustum();

            vsm_fbo.BindTexture(vsmTextures[j], UFramebufferAttachment::Color0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            URenderQueue render_queue;
            render->SetCurrentCamera(camera);
            scene->PrepareRenderQueue(render_queue);

            RenderQueue(render_queue, UBlendMode::Opaque, URenderPass::DepthShadow, camera);
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
            helper->GaussBlur(vsmTextures[j], 0.25f, vec2_x);
            helper->GaussBlur(vsmTextures[j], 0.25f, vec2_y);
        }
    }

    render->SetCurrentCamera(previous_camera);

    //glClearColor(0, 0, 0, 1);
}

void URenderManager::RenderQueue(const URenderQueue& render_queue, const UBlendMode blend_mode, const URenderPass type, const UCamera& camera)
{
    vector<int> keys;
    if (render_queue.data.find(blend_mode) != render_queue.data.end())
    {
        auto render_queue_blend_mode = render_queue.data.at(blend_mode);
        for (auto it = render_queue_blend_mode.begin(); it != render_queue_blend_mode.end(); ++it)
            keys.push_back(it->first);

        keys.erase(unique(keys.begin(), keys.end()), keys.end());

        std::sort(keys.begin(), keys.end());

        auto render = URenderer::GetInstance();

        render->SetCurrentCamera(camera);
        render->current_camera.UpdateFrustum();

        for (auto it = keys.begin(); it != keys.end(); ++it)
        {
            auto vec = render_queue_blend_mode.at(*it);
            for (auto mesh_it = vec.begin(); mesh_it != vec.end(); ++mesh_it)
            {
                render->model_view = (*mesh_it).first;
                (*mesh_it).second->Render(type);
            }
        }

        render->model_view = mat4_identity;
    }
}