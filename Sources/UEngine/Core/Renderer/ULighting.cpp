#include "ULighting.h"
#include "..\UEngine.h"

UDefferedLighting::UDefferedLighting()
{
	fb.Initialize();
	postfb.Initialize();

	lighting = dynamic_cast<UPostEffect*>(UEngine::rf.Load(UConfig::GetInstance()->GetParam("/xml/config/post_effects/deffered_lighting/"), UResourceType::PostEffect));

	colorScene = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	depthScene = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	normalScene = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	diffuseScene = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	//ambientScene = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	specularScene = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	resScene = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	resSceneA = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	resSceneB = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	positionScene = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));

	colorScene->name = "colorScene";
	depthScene->name = "depthScene";
	normalScene->name = "normalScene";
	diffuseScene->name = "diffuseScene";
	//ambientScene->name = "ambientScene";
	specularScene->name = "specularScene";
	positionScene->name = "positionScene";
	resScene->name = "colorScene";
	resSceneA->name = "previousScene";
	resSceneB->name = "previousScene";

	auto render = URenderer::GetInstance();

	colorScene->Create(render->GetWidth(), URenderer::GetInstance()->GetHeight(), UTextureFormat::RGBA);
	normalScene->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::RGBA);
	diffuseScene->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::RGBA);
	//ambientScene->Create(render->GetWidth(), render->GetHeight(), RGBA);
	specularScene->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::RGBA);
	resScene->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::RGBA);
	resSceneA->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::RGBA);
	resSceneB->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::RGBA);
	positionScene->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::R32F);
	depthScene->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::Depth32F);

	fb.BindTexture(depthScene, UFramebufferAttachment::Depth);
	fb.BindTexture(colorScene, UFramebufferAttachment::Color0);
	fb.BindTexture(normalScene, UFramebufferAttachment::Color1);
	fb.BindTexture(diffuseScene, UFramebufferAttachment::Color2);
	//fb.BindTexture(ambientScene, Color3);
	fb.BindTexture(specularScene, UFramebufferAttachment::Color3);
	fb.BindTexture(positionScene, UFramebufferAttachment::Color4);

	lighting->AddTexture(colorScene, 0);
	lighting->AddTexture(depthScene, 1);
	lighting->AddTexture(normalScene, 2);
	lighting->AddTexture(diffuseScene, 3);
	//lighting->AddTexture(ambientScene, 4);
	lighting->AddTexture(specularScene, 4);
	lighting->AddTexture(positionScene, 5);
}

UTexture* UDefferedLighting::Render(const UScene *scene, const UCamera camera, const URenderQueue& render_queue)
{
	auto render = URenderer::GetInstance();
	render->BindFBO(&fb);
	GLenum buffers[] = { (GLenum)UFramebufferAttachment::Color0,
					(GLenum)UFramebufferAttachment::Color1,
					(GLenum)UFramebufferAttachment::Color2,
					(GLenum)UFramebufferAttachment::Color3,
					(GLenum)UFramebufferAttachment::Color4 };

	glDrawBuffers(5, buffers);

	glViewport(0, 0, colorScene->GetWidth(), colorScene->GetHeight());
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_BACK);

	URenderManager::RenderQueue(render_queue, UBlendMode::Opaque, URenderPass::Deffered, camera);
	render->UnbindFBO();

	OPENGL_CHECK_FOR_ERRORS();

	auto lights = scene->GetLights();
	auto light_params = render->GetCurrentScene()->light_params;

	//Update frustum
	/*
	UVertex* vertices = reinterpret_cast<UVertex*>(lighting->vb.Lock());

	mat4 view = render->current_camera.GetView();
	mat4 viewProjection = render->current_camera.GetProjection() * view;
	mat4 viewProjectionInv = inverse(viewProjection);

	for (int i = 0; i < lighting->vb.GetNum(); i++)
	{
		vec3 pos = vertices[i].GetPosition();
		vec4 frustumCorner = vec4(pos.x, pos.y, 1.0f, 1.0f);
		frustumCorner = viewProjectionInv * frustumCorner;
		frustumCorner /= frustumCorner.w;

		vertices[i].SetNormal(frustumCorner);
	}

	lighting->vb.Unlock();
	*/

	int mod2 = 0;
	for (unsigned int i = 0; i < light_params.count; i++)
	{
		lighting->material.ClearUniformUnits();
		lighting->AddTexture(colorScene, 0);
		lighting->AddTexture(depthScene, 1);
		lighting->AddTexture(normalScene, 2);
		lighting->AddTexture(diffuseScene, 3);
		//lighting->AddTexture(ambientScene, 4);
		lighting->AddTexture(specularScene, 4);
		lighting->AddTexture(positionScene, 5);

		if (mod2 % 2 == 0)
		{
			lighting->AddTexture(resSceneA, 7);
		}
		else if (mod2 % 2 == 1)
		{
			lighting->AddTexture(resSceneB, 7);
		}

		lighting->material.params["lightIndex"] = UUniformParam((float)i);

		if (light_params.count == 1)
			lighting->material.params["state"] = UUniformParam(3.0f);
		else if (mod2 == 0)
			lighting->material.params["state"] = UUniformParam(0.0f);
		else if (mod2 == light_params.count - 1)
			lighting->material.params["state"] = UUniformParam(2.0f);
		else
			lighting->material.params["state"] = UUniformParam(1.0f);

		//////////////////////////////////////
		render->BindFBO(&postfb);
		postfb.BindTexture(mod2 == light_params.count - 1 ? resScene : (mod2 % 2 == 0 ? resSceneB : resSceneA), UFramebufferAttachment::Color0);

		OPENGL_CHECK_FOR_ERRORS();

		lighting->Render(URenderPass::Deffered, i);
		render->UnbindFBO();
		//////////////////////////////////////

		mod2++;
	}

	OPENGL_CHECK_FOR_ERRORS();
	return resScene;
}

UForwardLighting::UForwardLighting()
{
	fb.Initialize();
	postfb.Initialize();

	depthScene = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	normalScene = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
	resScene = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));

	depthScene->name = "depthScene";
	normalScene->name = "normalScene";
	resScene->name = "colorScene";

	auto render = URenderer::GetInstance();

	normalScene->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::RGBA_FLOAT);
	resScene->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::RGBA);
	depthScene->Create(render->GetWidth(), render->GetHeight(), UTextureFormat::Depth32F);

	fb.BindTexture(depthScene, UFramebufferAttachment::Depth);
	fb.BindTexture(resScene, UFramebufferAttachment::Color0);
	fb.BindTexture(normalScene, UFramebufferAttachment::Color1);
}

UTexture* UForwardLighting::Render(const UScene * scene, const UCamera camera, const URenderQueue& render_queue)
{
	auto render = URenderer::GetInstance();
	//color, depth
	render->BindFBO(&fb);
	fb.BindTexture(resScene, UFramebufferAttachment::Color0);
	fb.BindTexture(depthScene, UFramebufferAttachment::Depth);

	glViewport(0, 0, resScene->GetWidth(), resScene->GetHeight());
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_BACK);

	URenderManager::RenderQueue(render_queue, UBlendMode::Opaque, URenderPass::Forward, camera);
	URenderer::GetInstance()->UnbindFBO();

	// normal    
	URenderer::GetInstance()->BindFBO(&fb);
	fb.BindTexture(normalScene, UFramebufferAttachment::Color0);

	glViewport(0, 0, normalScene->GetWidth(), normalScene->GetHeight());
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	URenderManager::RenderQueue(render_queue, UBlendMode::Opaque, URenderPass::Normal, camera);
	URenderer::GetInstance()->UnbindFBO();
	return resScene;
}