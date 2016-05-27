#include "UScene.h"
#include "..\Renderer\URenderer.h"
#include "ULight.h"

//Process input keys
void UScene::KeysProccessing()
{
    // Camera moving
    int x_center = URenderer::GetInstance()->GetWidth() / 2, y_center = URenderer::GetInstance()->GetHeight() / 2;

    move_delta[0] = 0.5f * ((double)UInput::IsKeyDown('D') - (double)UInput::IsKeyDown('A'));
    move_delta[1] = 0.5f * ((double)UInput::IsKeyDown('S') - (double)UInput::IsKeyDown('W'));

    UInput::GetCursorPos(cursor_position, cursor_position + 1);
    rotate_delta[0] += (cursor_position[0] - x_center)*0.1;
    rotate_delta[1] += (cursor_position[1] - y_center)*0.1;

    static bool show_cursor = false;
    if (UInput::IsKeyPressed('L'))
        show_cursor = !show_cursor;

    if (!show_cursor)
        UInput::SetCursorPos(x_center, y_center);
    UInput::ShowCursor(show_cursor);
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

            light_params.position[cur] = position;
            light_params.ambient[cur] = ambient;
            light_params.diffuse[cur] = lights[cur]->GetDiffuse();
            light_params.specular[cur] = lights[cur]->GetSpecular();
            light_params.attenuation[cur] = vec3(lights[cur]->GetAttenuation());

            light_params.spotDirection[cur] = vec3(lights[cur]->GetSpotDirection());
            light_params.spotExponent[cur] = lights[cur]->GetSpotExponent();
            light_params.spotCosCutoff[cur] = lights[cur]->GetSpotCosCutoff();

            light_params.transforms[cur] = lights[cur]->GetLightTransform();
            light_params.types[cur] = lights[cur]->GetType() + lights[cur]->IsShadowCaster();

            light_params.light_index[cur] = i;

            cur++;
        }

        if (cur > MAX_LIGHTS)
            break;
    }
    light_params.count = cur;
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
