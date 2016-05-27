#include "UScene.h"
#include "..\Renderer\URenderer.h"
#include "ULight.h"

//Process input keys
void UScene::KeysProccessing()
{
    // Camera moving
    int xCenter = URenderer::GetInstance()->GetWidth() / 2, yCenter = URenderer::GetInstance()->GetHeight() / 2;

    move_delta[0] = 0.5f * ((double)UInput::IsKeyDown('D') - (double)UInput::IsKeyDown('A'));
    move_delta[1] = 0.5f * ((double)UInput::IsKeyDown('S') - (double)UInput::IsKeyDown('W'));

    UInput::GetCursorPos(cursor_position, cursor_position + 1);
    rotate_delta[0] += (cursor_position[0] - xCenter)*0.1;
    rotate_delta[1] += (cursor_position[1] - yCenter)*0.1;

    static bool showCursor = false;
    if (UInput::IsKeyPressed('L'))
        showCursor = !showCursor;

    if (!showCursor)
        UInput::SetCursorPos(xCenter, yCenter);
    UInput::ShowCursor(showCursor);
}

void UScene::UpdateLightParams()
{
    int cur = 0;
    for (unsigned int i = 0; i < lights.size(); i++)
    {
        if (lights[i] != NULL/* && lights[i]->castShadows*/)
        {
            vec4 position = lights[cur]->parent_transform * lights[cur]->local_transform.position;
            vec4 ambient = lights[cur]->GetAmbient();

            ambient.w = lights[cur]->GetShadowDistanceMin();
            position.w = lights[cur]->GetShadowDistanceMax();

            lightParams.position[cur] = position;
            lightParams.ambient[cur] = ambient;
            lightParams.diffuse[cur] = lights[cur]->GetDiffuse();
            lightParams.specular[cur] = lights[cur]->GetSpecular();
            lightParams.attenuation[cur] = vec3(lights[cur]->GetAttenuation());

            lightParams.spotDirection[cur] = vec3(lights[cur]->GetSpotDirection());
            lightParams.spotExponent[cur] = lights[cur]->GetSpotExponent();
            lightParams.spotCosCutoff[cur] = lights[cur]->GetSpotCosCutoff();

            lightParams.transforms[cur] = lights[cur]->GetLightTransform();
            lightParams.types[cur] = lights[cur]->GetType() + lights[cur]->IsShadowCaster();

            lightParams.light_index[cur] = i;

            cur++;
        }

        if (cur > MAX_LIGHTS)
            break;
    }
    lightParams.count = cur;
}

//Update with deltaTime
void UScene::Update(double deltaTime)
{
    //UInput::ShowCursor(false);

    //Camera movement
    URenderer::GetInstance()->main_ñamera.Rotate((float)rotate_delta[1], (float)rotate_delta[0], 0);
    URenderer::GetInstance()->main_ñamera.Move((float)move_delta[0], 0.0f, (float)move_delta[1]);

    rotate_delta[0] = rotate_delta[1] = 0;
    move_delta[0] = move_delta[1] = 0;

    //Nodes Update only one time per Global Update!
    if (root != NULL)
        root->Update(deltaTime);

    UpdateLightParams();
}

void UScene::Render(URENDER_PASS type, UCamera camera)
{
    auto render = URenderer::GetInstance();

    render->SetCurrentCamera(camera);
    render->current_camera.UpdateFrustum();

    if (root != NULL)
        root->Render(type);
    render->model = mat4_identity;
}

void UScene::Render(UMaterial *m, UCamera camera)
{
    auto render = URenderer::GetInstance();

    render->SetCurrentCamera(camera);
    render->current_camera.UpdateFrustum();

    if (root != NULL)
        root->Render(m);
    render->model = mat4_identity;
}

UScene::~UScene(void)
{
    Free();
}
