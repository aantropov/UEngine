#include "ULighting.h"
#include "..\UEngine.h"


UDefferedLighting:: UDefferedLighting(){
	fb.Initialize();
	postfb.Initialize();

	lighting = dynamic_cast<UPostEffect*>(UEngine::rf.Load(UConfig::GetInstance()->GetParam("/xml/config/post_effects/deffered_lighting/"), URESOURCE_POST_EFFECT));

	colorScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
	depthScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
	normalScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
	diffuseScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
	ambientScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
	specularScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
	resScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
	positionScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));

	colorScene->name = "colorScene";
	depthScene->name = "depthScene";
	normalScene->name = "normalScene";
	diffuseScene->name = "diffuseScene";
	ambientScene->name = "ambientScene";
	specularScene->name = "specularScene";
	positionScene->name = "positionScene";
	resScene->name = "colorScene";

	colorScene->Create(URenderer::GetInstance()->GetWidth(), URenderer::GetInstance()->GetHeight(), UTEXTURE_COLOR);
	normalScene->Create(URenderer::GetInstance()->GetWidth(), URenderer::GetInstance()->GetHeight(), UTEXTURE_FLOAT);
	diffuseScene->Create(URenderer::GetInstance()->GetWidth(), URenderer::GetInstance()->GetHeight(), UTEXTURE_COLOR);
	ambientScene->Create(URenderer::GetInstance()->GetWidth(), URenderer::GetInstance()->GetHeight(), UTEXTURE_COLOR);
	specularScene->Create(URenderer::GetInstance()->GetWidth(), URenderer::GetInstance()->GetHeight(), UTEXTURE_COLOR);
	resScene->Create(URenderer::GetInstance()->GetWidth(), URenderer::GetInstance()->GetHeight(), UTEXTURE_COLOR);
	positionScene->Create(URenderer::GetInstance()->GetWidth(), URenderer::GetInstance()->GetHeight(), UTEXTURE_FLOAT);
	depthScene->Create(URenderer::GetInstance()->GetWidth(), URenderer::GetInstance()->GetHeight(), UTEXTURE_DEPTH);
	

	fb.BindTexture(depthScene, UFB_ATTACHMENT_DEPTH);
	fb.BindTexture(colorScene, UFB_ATTACHMENT_COLOR0);
	fb.BindTexture(normalScene, UFB_ATTACHMENT_COLOR1);
	fb.BindTexture(diffuseScene, UFB_ATTACHMENT_COLOR2);
	fb.BindTexture(ambientScene, UFB_ATTACHMENT_COLOR3);
	fb.BindTexture(specularScene, UFB_ATTACHMENT_COLOR4);
	fb.BindTexture(positionScene, UFB_ATTACHMENT_COLOR5);
		
	lighting->AddTexture(colorScene, 0);
	lighting->AddTexture(depthScene, 1);
	lighting->AddTexture(normalScene, 2);
	lighting->AddTexture(diffuseScene, 3);
	lighting->AddTexture(ambientScene, 4);
	lighting->AddTexture(specularScene, 5);	
	lighting->AddTexture(positionScene, 6);	

}

UTexture* UDefferedLighting:: Render(UScene *scene)
{
	URenderer::GetInstance()->BindFBO(&fb);	
	GLenum buffers[] = {UFB_ATTACHMENT_COLOR0, UFB_ATTACHMENT_COLOR1, UFB_ATTACHMENT_COLOR2,UFB_ATTACHMENT_COLOR3,UFB_ATTACHMENT_COLOR4, UFB_ATTACHMENT_COLOR5};
	glDrawBuffers(6, buffers);

	glViewport(0, 0, colorScene->GetWidth(), colorScene->GetHeight());
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_BACK);

	scene->Render(URENDER_DEFFERED);
	URenderer::GetInstance()->UnbindFBO();

	//////////////////////////////////////
	URenderer::GetInstance()->BindFBO(&postfb);	
	postfb.BindTexture(resScene, UFB_ATTACHMENT_COLOR0);
	lighting->material.params["lightsNum"] = (float)(scene->GetLights().size());
	lighting->Render(URENDER_FORWARD);
	URenderer::GetInstance()->UnbindFBO();
	//////////////////////////////////////
	return resScene;

}

UForwardLighting:: UForwardLighting()
{
	fb.Initialize();
	postfb.Initialize();

	depthScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
	normalScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
	resScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
	
	depthScene->name = "depthScene";
	normalScene->name = "normalScene";
	resScene->name = "colorScene";

	normalScene->Create(URenderer::GetInstance()->GetWidth(), URenderer::GetInstance()->GetHeight(), UTEXTURE_FLOAT);
	resScene->Create(URenderer::GetInstance()->GetWidth(), URenderer::GetInstance()->GetHeight(), UTEXTURE_COLOR);
	depthScene->Create(URenderer::GetInstance()->GetWidth(), URenderer::GetInstance()->GetHeight(), UTEXTURE_DEPTH);
	

	fb.BindTexture(depthScene, UFB_ATTACHMENT_DEPTH);
	fb.BindTexture(resScene, UFB_ATTACHMENT_COLOR0);
	
}
UTexture* UForwardLighting:: Render(UScene *scene)
{
	//color, depth
	URenderer::GetInstance()->BindFBO(&fb);	
	fb.BindTexture(resScene, UFB_ATTACHMENT_COLOR0);
	fb.BindTexture(depthScene, UFB_ATTACHMENT_DEPTH);

	glViewport(0, 0, resScene->GetWidth(), resScene->GetHeight());
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_BACK);

	scene->Render(URENDER_FORWARD);
	URenderer::GetInstance()->UnbindFBO();
	
	// normal	
	URenderer::GetInstance()->BindFBO(&postfb);	
	postfb.BindTexture(normalScene, UFB_ATTACHMENT_COLOR0);

	glViewport(0, 0, normalScene->GetWidth(), normalScene->GetHeight());

	scene->Render(URENDER_NORMAL);
	URenderer::GetInstance()->UnbindFBO();

	return resScene;
}