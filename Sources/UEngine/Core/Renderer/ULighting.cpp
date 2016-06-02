#include "ULighting.h"
#include "..\UEngine.h"

ULighting::ULighting()
{
    auto render = URenderer::GetInstance();

    fb.Initialize();

    depth = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
    normal = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
    color = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));

    depth->name = "depthScene";
    normal->name = "normalScene";
    color->name = "colorScene";

    color->SetMipMap(false);
    normal->SetMipMap(false);
    depth->SetMipMap(false);

    normal->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::RGBA_Float);
    color->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::RGBA);
    depth->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::Depth32F);

    fb.BindTexture(depth, UFramebufferAttachment::Depth);
    fb.BindTexture(color, UFramebufferAttachment::Color0);
    fb.BindTexture(normal, UFramebufferAttachment::Color1);
}

UDefferedLightingOpaque::UDefferedLightingOpaque()
{
	accum_lighting.Initialize();
	deffered_lighting = dynamic_cast<UPostEffect*>(UEngine::rf.Load(UConfig::GetInstance()->GetParam("/xml/config/post_effects/deffered_lighting/"), UResourceType::PostEffect));

	diffuse = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	specular = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	res_color_ping = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	res_color_pong = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	position = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));

	diffuse->name = "diffuseScene";
	specular->name = "specularScene";
	position->name = "positionScene";
	res_color_ping->name = "previousScene";
	res_color_pong->name = "previousScene";

	auto render = URenderer::GetInstance();
  
    diffuse->SetMipMap(false);
    specular->SetMipMap(false);
    res_color_ping->SetMipMap(false);
    res_color_pong->SetMipMap(false);
    position->SetMipMap(false);

	diffuse->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::RGBA);
	specular->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::RGBA);
	res_color_ping->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::RGBA);
	res_color_pong->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::RGBA);
	position->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::R32F);

	fb.BindTexture(diffuse, UFramebufferAttachment::Color2);
	fb.BindTexture(specular, UFramebufferAttachment::Color3);
	fb.BindTexture(position, UFramebufferAttachment::Color4);

	deffered_lighting->AddTexture(color, 0);
	deffered_lighting->AddTexture(depth, 1);
	deffered_lighting->AddTexture(normal, 2);
	deffered_lighting->AddTexture(diffuse, 3);
	deffered_lighting->AddTexture(specular, 4);
	deffered_lighting->AddTexture(position, 5);
}

UForwardLightingTranslucent::UForwardLightingTranslucent(const UTexture* _depth)
{
}

UForwardLightingOpaque::UForwardLightingOpaque()
{
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
		deffered_lighting->material.ClearUniformUnits();
		deffered_lighting->AddTexture(color, 0);
		deffered_lighting->AddTexture(depth, 1);
		deffered_lighting->AddTexture(normal, 2);
		deffered_lighting->AddTexture(diffuse, 3);
		deffered_lighting->AddTexture(specular, 4);
		deffered_lighting->AddTexture(position, 5);

		deffered_lighting->AddTexture(ping_pong % 2 == 0 ? res_color_ping : res_color_pong, 7);

		deffered_lighting->material.params["lightIndex"] = UUniformParam((float)i);

		if (light_params.count == 1)
			deffered_lighting->material.params["state"] = UUniformParam(3.0f);
		else if (ping_pong == 0)
			deffered_lighting->material.params["state"] = UUniformParam(0.0f);
		else if (ping_pong == light_params.count - 1)
			deffered_lighting->material.params["state"] = UUniformParam(2.0f);
		else
			deffered_lighting->material.params["state"] = UUniformParam(1.0f);

		//////////////////////////////////////
		render->BindFBO(&accum_lighting);
		accum_lighting.BindTexture(ping_pong == light_params.count - 1 ? color : (ping_pong % 2 == 0 ? res_color_pong : res_color_ping), UFramebufferAttachment::Color0);

		OPENGL_CHECK_FOR_ERRORS();

		deffered_lighting->Render(URenderPass::Deffered, i);

		render->UnbindFBO();
		//////////////////////////////////////

		ping_pong++;
	}

	OPENGL_CHECK_FOR_ERRORS();
	return color;
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