#include "stdafx.h"
#define _UENGINE_IMPORT_
#include "..\UEngine\Core\UEngine.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    // Create Engine
    UEngine e;
    e.Initialize();
    
    UModel *sm = dynamic_cast<UModel*>(e.rf.Load("data\\Models\\knight_model.xml", URESOURCE_MODEL));
    UGameObject *gameObject = new UGameObject(sm);
    sm->animations["Test"]->StartAnimation(GetTickCount(), UANIMATION_PLAY_LOOP);
    //sm->animations["Test2"]->StartAnimation(GetTickCount(), UANIMATION_PLAY_LOOP);

    UModel *m = dynamic_cast<UModel*>(e.rf.Load("data\\Models\\scene_model.xml", URESOURCE_MODEL));
    
    ULight *light1 = new ULight(&e.rf, vec4(10.0f, 10.0f, 0.0f, 0.0f));
    ULight *light2 = new ULight(&e.rf, vec4(1.0f, 1.0f, 1.0f, 0.0f));
    ULight *light3 = new ULight(&e.rf, vec4(10.0f, 7.0f, 0.0f, 0.0f));
    ULight *light4 = new ULight(&e.rf, vec4(-12.0f, 9.0f, 10.0f, 0.0f));
    ULight *light5 = new ULight(&e.rf, vec4(-5.0f, 5.0f, 5.0f, 0.0f));
    ULight *light6 = new ULight(&e.rf, vec4(12.0f, 9.0f, 10.0f, 0.0f));
    ULight *light7 = new ULight(&e.rf, vec4(-5.0f, 4.0f, 5.0f, 0.0f));
    ULight *light8 = new ULight(&e.rf, vec4(-2.0f, 30.0f, 1.0f, 0.0f));
    
    UScript *script = dynamic_cast<UScript*>(e.rf.Create(URESOURCE_SCRIPT));
    script->Load("data\\Scripts\\test_script.xml");

    light8->AddComponent((UComponent*)script);
    light8->SetAttenuation(vec3(0.9f, 0.01f, 0.0f));
    light8->SetSpotCosCutoff(120.1f);
    light8->SetSpotExponent(10.0f);
    light8->SetSpotDirection(vec4(0.0f, -1.0f, 0.0f, 1.0f));
    
    script = dynamic_cast<UScript*>(e.rf.Create(URESOURCE_SCRIPT));
    script->Load("data\\Scripts\\test_script.xml");
    light7->AddComponent((UComponent*)script);

    script = dynamic_cast<UScript*>(e.rf.Create(URESOURCE_SCRIPT));
    script->Load("data\\Scripts\\test_script.xml");
    light6->AddComponent((UComponent*)script);

    script = dynamic_cast<UScript*>(e.rf.Create(URESOURCE_SCRIPT));
    script->Load("data\\Scripts\\test_script.xml");
    light5->AddComponent((UComponent*)script);

    script = dynamic_cast<UScript*>(e.rf.Create(URESOURCE_SCRIPT));
    script->Load("data\\Scripts\\test_script.xml");
    light1->AddComponent((UComponent*)script);


    light1->castShadows = true;
    light2->castShadows = true;
    light3->castShadows = true;
    light4->castShadows = true;
    light5->castShadows = true;
    light6->castShadows = true;
    light7->castShadows = true;
    light8->castShadows = true;

    //UModel *skybox_s = dynamic_cast<UModel*>(e.rf.Load("data\\skybox.xml", URESOURCE_MODEL));
    
    // Main node in the scene
    UScene scene;
    UScene::USceneNode *node = new UScene::USceneNode(new UGameObject());
    
    //node->AddChild(new UScene::USceneNode(new UGameObject(skybox_s)));
    node->AddChild(new UScene::USceneNode(gameObject));

    auto go = new UScene::USceneNode(light1);    
    /*go->AddChild(new UScene::USceneNode(light2));
    node->AddChild(go);
    node->AddChild(new UScene::USceneNode(light3));
    node->AddChild(new UScene::USceneNode(light4));*/
    node->AddChild(new UScene::USceneNode(light5));
    node->AddChild(new UScene::USceneNode(light6));
    node->AddChild(new UScene::USceneNode(light7));
    node->AddChild(new UScene::USceneNode(light8));
    
    node->AddChild(new UScene::USceneNode(new UGameObject(m)));

    /*scene.AddLight(light1);
    scene.AddLight(light2);
    scene.AddLight(light3);
    scene.AddLight(light4);*/
    scene.AddLight(light5);
    scene.AddLight(light6);
    scene.AddLight(light7);
    scene.AddLight(light8);/**/
    
    scene.root = node;
    
    /*UModel *m1[10];
    
    for(int i = 0; i <10; i++)
    {
        m1[i] = dynamic_cast<UModel*>(e.rf.Load("data\\fallout3_model.xml", URESOURCE_MODEL));
        m1[i]->animations["Test"]->StartAnimation(GetTickCount(), UANIMATION_PLAY_ONCE);
        //m1[i]->Load("data\\light.xml");
        node->AddChild(new UScene::USceneNode(new UGameObject(m1[i])));
    }
    */
    
    e.SetCurrentScene(&scene);
    e.Run();
    e.Release();

    return 0;
}