#include "stdafx.h"
#define _UENGINE_IMPORT_
#include "..\UEngine\Core\UEngine.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    UEngine e;
    e.Initialize();

    UModel *test_model = dynamic_cast<UModel*>(e.rf.Load("data\\test\\test_model.xml", URESOURCE_MODEL));
    UGameObject *gameObject = new UGameObject(test_model);
    //test_model->animations["Test"]->StartAnimation(GetTickCount(), UANIMATION_PLAY_LOOP);

    // Main node in the scene
    UScene scene;
    UScene::USceneNode *node = new UScene::USceneNode(new UGameObject());
    node->AddChild(new UScene::USceneNode(new UGameObject(test_model)));
    scene.root = node;

    UScript *script = dynamic_cast<UScript*>(e.rf.Create(URESOURCE_SCRIPT));
    script->LoadFromFile("data\\Scripts\\test_script.xml");

    vec3 atten = vec3(0.f, 0.004f, 0.f);

    ULight *light = new ULight(&e.rf, vec4(-20.0f, 20.0f, 0.0f, 0.0f));
    light->SetAttenuation(atten);
    light->SetSpotExponent(10.0f);
    light->SetSpotCosCutoff(45.0f);
    light->castShadows = true;
    //light->SetDiffuse(vec4_y);

    node->AddChild(new UScene::USceneNode(light));
    scene.AddLight(light);
    light->AddComponent((UComponent*)script);

    for (int i = 0; i < 0; i++)
    {
        ULight *additional_light = new ULight(&e.rf, vec4(rand() % 20 - 10.0f, rand() % 20, rand() % 20 - 10.0f, 0.0f));
        additional_light->SetAttenuation(atten);
        additional_light->castShadows = true;
        additional_light->SetSpotExponent(2);
        additional_light->SetDiffuse(vec4((rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f, 1));
        node->AddChild(new UScene::USceneNode(additional_light));
        scene.AddLight(additional_light);
        script = dynamic_cast<UScript*>(e.rf.Create(URESOURCE_SCRIPT));
        script->LoadFromFile("data\\Scripts\\test_script.xml");
        additional_light->AddComponent((UComponent*)script);
    }

    /*
    auto go = new UScene::USceneNode(light1);
    go->AddChild(new UScene::USceneNode(light2));
    node->AddChild(go);
    node->AddChild(new UScene::USceneNode(light3));
    node->AddChild(new UScene::USceneNode(light4));
    node->AddChild(new UScene::USceneNode(light5));
    node->AddChild(new UScene::USceneNode(light6));
    node->AddChild(new UScene::USceneNode(light7));
    node->AddChild(new UScene::USceneNode(light8));

    /*UModel *m = dynamic_cast<UModel*>(e.rf.Load("data\\Models\\scene_model.xml", URESOURCE_MODEL));

    ULight *light1 = new ULight(&e.rf, vec4(10.0f, 10.0f, 0.0f, 0.0f));
    ULight *light2 = new ULight(&e.rf, vec4(1.0f, 1.0f, 1.0f, 0.0f));
    ULight *light3 = new ULight(&e.rf, vec4(10.0f, 7.0f, 0.0f, 0.0f));
    ULight *light4 = new ULight(&e.rf, vec4(-12.0f, 10.0f, 10.0f, 0.0f));
    ULight *light5 = new ULight(&e.rf, vec4(-5.0f, 5.0f, 5.0f, 0.0f));
    ULight *light6 = new ULight(&e.rf, vec4(12.0f, 9.0f, 10.0f, 0.0f));
    ULight *light7 = new ULight(&e.rf, vec4(-5.0f, 4.0f, 5.0f, 0.0f));
    ULight *light8 = new ULight(&e.rf, vec4(-2.0f, 30.0f, 1.0f, 0.0f));

    UScript *script = dynamic_cast<UScript*>(e.rf.Create(URESOURCE_SCRIPT));
    script->LoadFromFile("data\\Scripts\\test_script.xml");

    light8->SetAttenuation(vec3(0.0f, 0.004f, 0.0f));
    light8->AddComponent((UComponent*)script);
    light8->SetSpotCosCutoff(120.1f);
    light8->SetSpotExponent(10.0f);
    light8->SetSpotDirection(vec4(0.0f, -1.0f, 0.0f, 1.0f));

    script = dynamic_cast<UScript*>(e.rf.Create(URESOURCE_SCRIPT));
    script->LoadFromFile("data\\Scripts\\test_script.xml");
    light7->AddComponent((UComponent*)script);

    script = dynamic_cast<UScript*>(e.rf.Create(URESOURCE_SCRIPT));
    script->LoadFromFile("data\\Scripts\\test_script.xml");
    light6->AddComponent((UComponent*)script);

    script = dynamic_cast<UScript*>(e.rf.Create(URESOURCE_SCRIPT));
    script->LoadFromFile("data\\Scripts\\test_script.xml");
    light5->AddComponent((UComponent*)script);

    script = dynamic_cast<UScript*>(e.rf.Create(URESOURCE_SCRIPT));
    script->LoadFromFile("data\\Scripts\\test_script.xml");
    light1->AddComponent((UComponent*)script);

    light8->castShadows = true;

    UModel *skybox_s = dynamic_cast<UModel*>(e.rf.Load("data\\Models\\skybox_model.xml", URESOURCE_MODEL));

    node->AddChild(new UScene::USceneNode(new UGameObject(skybox_s)));
    node->AddChild(new UScene::USceneNode(gameObject));

    auto go = new UScene::USceneNode(light1);
    go->AddChild(new UScene::USceneNode(light2));
    node->AddChild(go);
    node->AddChild(new UScene::USceneNode(light3));
    node->AddChild(new UScene::USceneNode(light4));
    node->AddChild(new UScene::USceneNode(light5));
    node->AddChild(new UScene::USceneNode(light6));
    node->AddChild(new UScene::USceneNode(light7));
    node->AddChild(new UScene::USceneNode(light8));

    node->AddChild(new UScene::USceneNode(new UGameObject(m)));

    /*scene.AddLight(light1);
    scene.AddLight(light2);
    scene.AddLight(light3);
    scene.AddLight(light4);
    scene.AddLight(light5);
    scene.AddLight(light6);
    scene.AddLight(light7);
    scene.AddLight(light8);
    scene.root = node;
    */

    e.SetCurrentScene(&scene);
    e.Run();
    e.Release();

    return 0;
}