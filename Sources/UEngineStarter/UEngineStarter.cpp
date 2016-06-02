#include "stdafx.h"
#define _UENGINE_IMPORT_
#include "..\UEngine\Core\UEngine.h"
#include "USkyBoxComponent.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    UEngine e;
    e.Initialize();

    UModel *test_model = dynamic_cast<UModel*>(e.rf.Load("data\\test\\test_model.xml", UResourceType::Model));
    UModel *sky_sphere_model = dynamic_cast<UModel*>(e.rf.Load("data\\test\\sky_sphere_model.xml", UResourceType::Model));
    UModel *knight_model = dynamic_cast<UModel*>(e.rf.Load("data\\models\\knight_model.xml", UResourceType::Model));
    
    const int maxObjs = 4;
    UGameObject *gameObject[maxObjs];

    UScene scene;
    scene.root = new UScene::USceneNode(new UGameObject());

    auto sky = new UGameObject(sky_sphere_model);
    sky->AddComponent(new USkyboxComponent());
    scene.root->AddChild(new UScene::USceneNode(sky));

    knight_model->animations["Test"]->StartAnimation(GetTickCount(), UPlayState::PlayOnce);

    for (int i = 0; i < sqrt(maxObjs); i++)
        for (int j = 0; j < sqrt(maxObjs); j++)
        {          
            float offset = (float)sqrt(maxObjs) / 2.0f;
            gameObject[i + i*j] = new UGameObject(test_model);
            gameObject[i + i*j]->local_transform.position = vec4_x * 77 * ((float)i - offset) + vec4_z * 77 * ((float)j - offset);
            scene.root->AddChild(new UScene::USceneNode(gameObject[i + i*j]));
        }

    scene.root->AddChild(new UScene::USceneNode(new UGameObject(knight_model)));

    UScript *script = dynamic_cast<UScript*>(e.rf.Create(UResourceType::Script));
    script->LoadFromFile("data\\Scripts\\test_script.xml");

    vec3 atten = vec3(0.0f, 0.001f, 0.00002f);
    vec4 rand_color = vec4(rand() % 1000, rand() % 1000, rand() % 1000, rand() % 1000) * 0.001f;
    rand_color = vec4_one;
    ULight *light = new ULight(&e.rf, vec4(-100.0f, 100.0f, 0.0f, 1.0f));

    light->SetAttenuation(atten * 2.3);
    light->SetAmbient(vec4_one * 0.0025f);
    light->IsShadowCaster(true);
    light->SetType(ULightType::Directional);
    light->SetDiffuse(rand_color);

    scene.root->AddChild(new UScene::USceneNode(light));
    scene.AddLight(light);
    //light->AddComponent((UComponent*)script);

    for (int i = 0; i < 1; i++)
    {
        vec4 rand_pos = vec4(rand() % 40 - 80, rand() % 20 + 30, rand() % 40 - 80, rand() % 80);
        rand_color = vec4(rand() % 1000, rand() % 1000, rand() % 1000, rand() % 1000)* 0.001f;

        ULight *additional_light = new ULight(&e.rf, rand_pos);
        additional_light->SetAttenuation(atten * 1.5);
        additional_light->IsShadowCaster(true);

        additional_light->SetSpotExponent(100.0f);
        additional_light->SetSpotCosCutoff(50);

        additional_light->SetDiffuse(rand_color);
        additional_light->SetAmbient(vec4_x * 0.01f);
        additional_light->SetType(ULightType::Spot);

        scene.root->AddChild(new UScene::USceneNode(additional_light));
        scene.AddLight(additional_light);
        script = dynamic_cast<UScript*>(e.rf.Create(UResourceType::Script));
        script->LoadFromFile("data\\Scripts\\test_script.xml");
        //additional_light->AddComponent((UComponent*)script);
    }

    e.SetCurrentScene(&scene);
    e.Run();
    e.Release();

    return 0;
}