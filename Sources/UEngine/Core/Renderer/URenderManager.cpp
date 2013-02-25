#include "URenderManager.h"
#include "URenderer.h"
#include "..\Basic\UScene.h"
#include "..\Utils\enum.h"
#include "..\Basic\ULight.h"
#include "..\UEngine.h"


URenderManager::URenderManager(){

	if(atoi(UConfig::GetInstance()->GetParam("/xml/config/deffered_lighting/").c_str()) == 1)
		lighting = new UDefferedLighting();
	else
		lighting = new UForwardLighting();

	depthFbo.Initialize();
	postEffectFbo.Initialize();	
	postpostEffectFbo.Initialize();	

	postEffectSSAO = dynamic_cast<UPostEffect*>(UEngine::rf.Load("data\\\post_effect_ssao.xml", URESOURCE_POST_EFFECT));
	postEffectRipple = dynamic_cast<UPostEffect*>(UEngine::rf.Load("data\\\post_effect_blur.xml", URESOURCE_POST_EFFECT));
	
	posteffectScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));

	posteffectScene->Create(URenderer::GetInstance()->GetWidth(), URenderer::GetInstance()->GetHeight(), UTEXTURE_COLOR);
	posteffectScene->name = "colorScene";

	postEffectRipple->AddTexture(lighting->resScene, 0);
	postEffectRipple->AddTexture(lighting->depthScene, 1);

	postEffectSSAO->AddTexture(lighting->resScene, 2);
	postEffectSSAO->AddTexture(lighting->depthScene, 1);
	postEffectSSAO->AddTexture(lighting->normalScene, 3);
	
	depthTextureSize = atoi(UConfig::GetInstance()->GetParam("/xml/config/depth_texture_size/").c_str());
	
}


URenderManager::~URenderManager(void){
	delete lighting;
}

void URenderManager::Render(UScene* scene){

	UCamera previousCam = URenderer::GetInstance()->GetCamera();
	
	URenderer::GetInstance()->BindFBO(&depthFbo);
	
	glViewport(0, 0, depthTextureSize, depthTextureSize);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	//glDepthMask(GL_TRUE);	
	glCullFace(GL_FRONT);
	
	auto lights = scene->GetLights();
	for(unsigned int i = 0; i < lights.size(); i++)
	{	
		if(!lights[i]->castShadows)
			continue;

		auto depthTextures = lights[i]->GetDepthTextures();
		for(unsigned int j = 0; j < depthTextures.size(); j++){
			
			depthFbo.BindTexture(depthTextures[j], UFB_ATTACHMENT_DEPTH);
			glClear(GL_DEPTH_BUFFER_BIT);	

			URenderer::GetInstance()->SetCamera(lights[i]->GetCameras()[j]);					
			scene->Render(URENDER_DEPTH);
		}
	}

	URenderer::GetInstance()->SetCamera(previousCam);
	URenderer::GetInstance()->UnbindFBO();
	
	OPENGL_CHECK_FOR_ERRORS();
	
	lighting->Render(scene);
	postEffectSSAO->Render(URENDER_FORWARD);
	//postEffectRipple->Render(URENDER_FORWARD);
}