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
	post_effect_fbo.Initialize();
	post_post_effect_fbo.Initialize();

    color_bloom = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
    color_bloom->name = "colorScene";
    color_bloom->Create(512, 512, UTextureFormat::RGBA);

	postEffectSSAO = dynamic_cast<UPostEffect*>(UEngine::rf.Load("data\\PostEffects\\post_effect_ssao.xml", UResourceType::PostEffect));
	postEffectDOF = dynamic_cast<UPostEffect*>(UEngine::rf.Load("data\\PostEffects\\post_effect_dof.xml", UResourceType::PostEffect));
	postEffectRipple = dynamic_cast<UPostEffect*>(UEngine::rf.Load("data\\PostEffects\\post_effect_blur.xml", UResourceType::PostEffect));

	posteffectScene = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));

	posteffectScene->Create(URenderer::GetInstance()->GetWidth(), URenderer::GetInstance()->GetHeight(), UTextureFormat::RGBA);
	posteffectScene->name = "colorScene";

	postEffectRipple->AddTexture(opaque_lighting->color, 0);
	postEffectRipple->AddTexture(opaque_lighting->depth, 1);

	postEffectDOF->AddTexture(opaque_lighting->color, 0);
	postEffectDOF->AddTexture(opaque_lighting->depth, 1);

	postEffectSSAO->AddTexture(opaque_lighting->color, 2);
	postEffectSSAO->AddTexture(opaque_lighting->depth, 1);
	postEffectSSAO->AddTexture(opaque_lighting->normal, 3);

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
	URenderQueue render_queue;
	auto render = URenderer::GetInstance();

	scene->PrepareRenderQueue(render_queue);
	
	RenderShadowMaps();

	OPENGL_CHECK_FOR_ERRORS();

	opaque_lighting->Render(scene, render->main_ñamera, render_queue);
    //URendererHelper::GetInstance()->CopyTexture(opaque_lighting->color, color_bloom);
    
	//translucent_lighting->Render(scene, render->main_ñamera, render_queue);

    //postEffectRipple->AddTexture(color_bloom, 0);
	//postEffectDOF->Render(URENDER_FORWARD);
    //postEffectRipple->AddTexture(lights[light_params.light_index[1]]->GetDepthTextures()[0], 0);
	//postEffectSSAO->Render(URENDER_FORWARD);

    //glViewport(0, 0, render->GetWidth(), 1024);
	postEffectRipple->Render(URenderPass::Forward);
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
	if(render_queue.data.find(blend_mode) != render_queue.data.end())
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