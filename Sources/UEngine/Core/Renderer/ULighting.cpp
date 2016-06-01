#include "ULighting.h"
#include "..\UEngine.h"

UDefferedLightingOpaque::UDefferedLightingOpaque()
{
	fb.Initialize();
	post_effect_fb.Initialize();

	post_effect = dynamic_cast<UPostEffect*>(UEngine::rf.Load(UConfig::GetInstance()->GetParam("/xml/config/post_effects/deffered_lighting/"), UResourceType::PostEffect));

	color = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	depth = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	normal = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	diffuse = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	specular = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	res_color_ping = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	res_color_pong = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	position = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));

  	color->name = "colorScene";
	depth->name = "depthScene";
	normal->name = "normalScene";
	diffuse->name = "diffuseScene";
	specular->name = "specularScene";
	position->name = "positionScene";
	res_color_ping->name = "previousScene";
	res_color_pong->name = "previousScene";

	auto render = URenderer::GetInstance();
  
    color->SetMipMap(false);
	color->Create(render->GetWidth(), URenderer::GetInstance()->GetHeight(), UTextureFormat::RGBA);
	normal->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::RGBA);
	diffuse->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::RGBA);
	specular->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::RGBA);
	res_color_ping->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::RGBA);
	res_color_pong->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::RGBA);
	position->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::R32F);
	depth->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::Depth32F);

	fb.BindTexture(depth, UFramebufferAttachment::Depth);
	fb.BindTexture(color, UFramebufferAttachment::Color0);
	fb.BindTexture(normal, UFramebufferAttachment::Color1);
	fb.BindTexture(diffuse, UFramebufferAttachment::Color2);
	fb.BindTexture(specular, UFramebufferAttachment::Color3);
	fb.BindTexture(position, UFramebufferAttachment::Color4);

	post_effect->AddTexture(color, 0);
	post_effect->AddTexture(depth, 1);
	post_effect->AddTexture(normal, 2);
	post_effect->AddTexture(diffuse, 3);
	post_effect->AddTexture(specular, 4);
	post_effect->AddTexture(position, 5);
}

UTexture* UDefferedLightingOpaque::Render(const UScene *scene, const UCamera camera, const URenderQueue& render_queue)
{
	auto render = URenderer::GetInstance();
	render->BindFBO(&fb);
	GLenum buffers[] = { (GLenum)UFramebufferAttachment::Color0,
					(GLenum)UFramebufferAttachment::Color1,
					(GLenum)UFramebufferAttachment::Color2,
					(GLenum)UFramebufferAttachment::Color3,
					(GLenum)UFramebufferAttachment::Color4 };

	glDrawBuffers(5, buffers);

	glViewport(0, 0, color->GetWidth(), color->GetHeight());
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_BACK);

	URenderManager::RenderQueue(render_queue, UBlendMode::Opaque, URenderPass::Deffered, camera);

	glDrawBuffers(1, buffers);

	URenderManager::RenderQueue(render_queue, UBlendMode::Additive, URenderPass::Forward, camera);

	render->UnbindFBO();

	OPENGL_CHECK_FOR_ERRORS();

	auto lights = scene->GetLights();
	auto light_params = render->GetCurrentScene()->light_params;

	int ping_pong = 0;
	for (unsigned int i = 0; i < light_params.count; i++)
	{
		post_effect->material.ClearUniformUnits();
		post_effect->AddTexture(color, 0);
		post_effect->AddTexture(depth, 1);
		post_effect->AddTexture(normal, 2);
		post_effect->AddTexture(diffuse, 3);
		post_effect->AddTexture(specular, 4);
		post_effect->AddTexture(position, 5);

		post_effect->AddTexture(ping_pong % 2 == 0 ? res_color_ping : res_color_pong, 7);

		post_effect->material.params["lightIndex"] = UUniformParam((float)i);

		if (light_params.count == 1)
			post_effect->material.params["state"] = UUniformParam(3.0f);
		else if (ping_pong == 0)
			post_effect->material.params["state"] = UUniformParam(0.0f);
		else if (ping_pong == light_params.count - 1)
			post_effect->material.params["state"] = UUniformParam(2.0f);
		else
			post_effect->material.params["state"] = UUniformParam(1.0f);

		//////////////////////////////////////
		render->BindFBO(&post_effect_fb);
		post_effect_fb.BindTexture(ping_pong == light_params.count - 1 ? color : (ping_pong % 2 == 0 ? res_color_pong : res_color_ping), UFramebufferAttachment::Color0);

		OPENGL_CHECK_FOR_ERRORS();

		post_effect->Render(URenderPass::Deffered, i);

		render->UnbindFBO();
		//////////////////////////////////////

		ping_pong++;
	}

	OPENGL_CHECK_FOR_ERRORS();
	return color;
}

UForwardLightingOpaque::UForwardLightingOpaque()
{
	fb.Initialize();

	depth = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	normal = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	color = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));

	depth->name = "depthScene";
	normal->name = "normalScene";
	color->name = "colorScene";

	auto render = URenderer::GetInstance();

    color->SetMipMap(false);
	normal->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::RGBA_Float);
	color->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::RGBA);
	depth->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::Depth32F);

	fb.BindTexture(depth, UFramebufferAttachment::Depth);
	fb.BindTexture(color, UFramebufferAttachment::Color0);
	fb.BindTexture(normal, UFramebufferAttachment::Color1);
}

UTexture* UForwardLightingOpaque::Render(const UScene * scene, const UCamera camera, const URenderQueue& render_queue)
{
	auto render = URenderer::GetInstance();
	//color, depth
	render->BindFBO(&fb);
	fb.BindTexture(color, UFramebufferAttachment::Color0);
	fb.BindTexture(depth, UFramebufferAttachment::Depth);

	glViewport(0, 0, color->GetWidth(), color->GetHeight());
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_BACK);

	URenderManager::RenderQueue(render_queue, UBlendMode::Opaque, URenderPass::Forward, camera);
	URenderManager::RenderQueue(render_queue, UBlendMode::Additive, URenderPass::Forward, camera);

	URenderer::GetInstance()->UnbindFBO();

	// normal    
	URenderer::GetInstance()->BindFBO(&fb);
	fb.BindTexture(normal, UFramebufferAttachment::Color0);

	glViewport(0, 0, normal->GetWidth(), normal->GetHeight());

	URenderManager::RenderQueue(render_queue, UBlendMode::Opaque, URenderPass::Normal, camera);
	URenderer::GetInstance()->UnbindFBO();
	return color;
}

UForwardLightingTranslucent::UForwardLightingTranslucent(const UTexture* _depth)
{
	auto render = URenderer::GetInstance();

	fb.Initialize();

	color = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	color->name = "colorScene";
	color->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::RGBA);

	fb.BindTexture(_depth, UFramebufferAttachment::Depth);
	fb.BindTexture(color, UFramebufferAttachment::Color0);
}

UTexture* UForwardLightingTranslucent::Render(const UScene * scene, const UCamera camera, const URenderQueue& render_queue)
{
	auto render = URenderer::GetInstance();
	//color, depth
	render->BindFBO(&fb);

	glViewport(0, 0, color->GetWidth(), color->GetHeight());
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glCullFace(GL_BACK);

	URenderManager::RenderQueue(render_queue, UBlendMode::Translucent, URenderPass::Forward, camera);
	URenderer::GetInstance()->UnbindFBO();

	return color;
}