#include "stdafx.h"

#define _UENGINE_IMPORT_
#include "..\UEngine\Core\UEngine.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// Create Engine
	UEngine e;
	e.Initialize();
	
	
	UModel *sm = dynamic_cast<UModel*>(e.rf.Load("data\\fallout3_model.xml", URESOURCE_MODEL));
	UModel *m = dynamic_cast<UModel*>(e.rf.Load("data\\test_model.xml", URESOURCE_MODEL));

	sm->animations["Test"]->StartAnimation(GetTickCount(), UANIMATION_PLAY_LOOP);
	sm->animations["Test2"]->StartAnimation(GetTickCount(), UANIMATION_PLAY_LOOP);

	ULight *light1 = new ULight(&e.rf, vec4(10.0f, 10.0f, 0.0f, 0.0f));
	ULight *light2 = new ULight(&e.rf, vec4(1.0f, 1.0f, 1.0f, 0.0f));
	ULight *light3 = new ULight(&e.rf, vec4(10.0f, 7.0f, 0.0f, 0.0f));
	ULight *light4 = new ULight(&e.rf, vec4(-12.0f, 9.0f, 10.0f, 0.0f));
	ULight *light5 = new ULight(&e.rf, vec4(-5.0f, 5.0f, 5.0f, 0.0f));
	ULight *light6 = new ULight(&e.rf, vec4(12.0f, 9.0f, 10.0f, 0.0f));
	ULight *light7 = new ULight(&e.rf, vec4(-5.0f, 4.0f, 5.0f, 0.0f));
	ULight *light8 = new ULight(&e.rf, vec4(-2.0f, 25.0f, 1.0f, 0.0f));
	
	light8->SetAttenuation(vec3(0.75f, 0.0f, 0.0f));
	light8->SetSpotCosCutoff(70.0f);
	light8->SetSpotExponent(5.0f);

	light1->castShadows = true;
	light2->castShadows = true;
	light3->castShadows = true;
	light4->castShadows = true;
	light5->castShadows = true;
	light6->castShadows = true;
	light7->castShadows = true;
	light8->castShadows = true;
	/**/
	UModel *skybox_s = dynamic_cast<UModel*>(e.rf.Load("data\\skybox.xml", URESOURCE_MODEL));
	
	// Main node in the scene
	UScene scene;
	UScene::USceneNode *node = new UScene::USceneNode(new UGameObject());
	
	node->AddChild(new UScene::USceneNode(new UGameObject(skybox_s)));
	node->AddChild(new UScene::USceneNode(new UGameObject(m)));

	auto go = new UScene::USceneNode(light1);	
	go->AddChild(new UScene::USceneNode(light2));
		
	light1->r.x = 1.0f;
	node->AddChild(go);

	node->AddChild(new UScene::USceneNode(light3));
	node->AddChild(new UScene::USceneNode(light4));
	node->AddChild(new UScene::USceneNode(light5));
	node->AddChild(new UScene::USceneNode(light6));
	node->AddChild(new UScene::USceneNode(light7));
	node->AddChild(new UScene::USceneNode(light8));

	node->AddChild(new UScene::USceneNode(new UGameObject(sm)));

	scene.AddLight(light1);
	scene.AddLight(light2);
	scene.AddLight(light3);
	scene.AddLight(light4);
	scene.AddLight(light5);
	scene.AddLight(light6);
	scene.AddLight(light7);
	scene.AddLight(light8);
	
	scene.root = node;
	
	//ULight l[30];
	/*UModel *m1[1000];

	for(int i = 0; i <0; i++)
	{

		m1[i] = dynamic_cast<UModel*>(e.rf.Load("data\\fallout3_model.xml", URESOURCE_MODEL));
		m1[i]->animations["Test"]->StartAnimation(GetTickCount(), UANIMATION_PLAY_ONCE);
		//m1[i]->Load("data\\light.xml");
		node->AddChild(new UScene::USceneNode(m1[i]));

		/*l[i] = ULight(&e.rf, vec4(0.0f, i, -10.0f, 0.0f));
		node->AddChild(new UScene::USceneNode(&l[i]));
		scene.AddLight(&l[i]);*/
	/*}
*/
	e.SetCurrentScene(&scene);
	e.Run();
	e.Release();

	return 0;
}
