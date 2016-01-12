#include "stdafx.h"
#define _UENGINE_IMPORT_
#include "..\UEngine\Core\UEngine.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    UEngine e;
    e.Initialize();

    UModel *sm = dynamic_cast<UModel*>(e.rf.Load("data\\Models\\knight_model.xml", URESOURCE_MODEL));
    UGameObject *gameObject = new UGameObject(sm);
    sm->animations["Test"]->StartAnimation(GetTickCount(), UANIMATION_PLAY_LOOP);

    UModel *m = dynamic_cast<UModel*>(e.rf.Load("data\\Models\\scene_model.xml", URESOURCE_MODEL));

    ULight *light1 = new ULight(&e.rf, vec4(10.0f, 30.0f, 0.0f, 0.0f));
    light1->SetAmbient(vec4(0.5f, 0.5f, 0.5f, 1.0f));
    light1->SetDiffuse(vec4(0.5f, 0.5f, 0.5f, 1.0f));
    light1->SetAttenuation(vec3(0.0175f, 0.001f, 0.0000001f));
    light1->castShadows = true;

    UModel *skybox_s = dynamic_cast<UModel*>(e.rf.Load("data\\Models\\skybox_model.xml", URESOURCE_MODEL));

    // Main node in the scene
    UScene scene;
    UScene::USceneNode *node = new UScene::USceneNode(new UGameObject());

    node->AddChild(new UScene::USceneNode(new UGameObject(skybox_s)));
    node->AddChild(new UScene::USceneNode(gameObject));

    auto go = new UScene::USceneNode(light1);

    node->AddChild(new UScene::USceneNode(new UGameObject(m)));

    scene.AddLight(light1);
    scene.root = node;

    e.SetCurrentScene(&scene);
    e.Run();
    e.Release();

    return 0;
}