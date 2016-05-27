#include "UScene.h"
#include "..\Renderer\URenderer.h"
#include "ULight.h"

//Process input keys
void UScene::KeysProccessing()
{
    // Camera moving
    int xCenter = URenderer::GetInstance()->GetWidth() / 2, yCenter = URenderer::GetInstance()->GetHeight() / 2;

    moveDelta[0] = 0.5f * ((double)UInput::IsKeyDown('D') - (double)UInput::IsKeyDown('A'));
    moveDelta[1] = 0.5f * ((double)UInput::IsKeyDown('S') - (double)UInput::IsKeyDown('W'));

    UInput::GetCursorPos(cursorPos, cursorPos + 1);
    rotateDelta[0] += (cursorPos[0] - xCenter)*0.1;
    rotateDelta[1] += (cursorPos[1] - yCenter)*0.1;

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
            vec4 position = lights[cur]->world * lights[cur]->local.position;
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

            lightParams.lightIndex[cur] = i;

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
    URenderer::GetInstance()->mainCamera.Rotate((float)rotateDelta[1], (float)rotateDelta[0], 0);
    URenderer::GetInstance()->mainCamera.Move((float)moveDelta[0], 0.0f, (float)moveDelta[1]);

    rotateDelta[0] = rotateDelta[1] = 0;
    moveDelta[0] = moveDelta[1] = 0;

    //Nodes Update only one time per Global Update!
    if (root != NULL)
        root->Update(deltaTime);

    UpdateLightParams();
}

void UScene::Render(URENDER_PASS type, UCamera camera)
{
    auto render = URenderer::GetInstance();

    render->SetCurrentCamera(camera);
    render->currentCamera.UpdateFrustum();

    if (root != NULL)
        root->Render(type);
    render->model = mat4_identity;
}

void UScene::Render(UMaterial *m, UCamera camera)
{
    auto render = URenderer::GetInstance();

    render->SetCurrentCamera(camera);
    render->currentCamera.UpdateFrustum();

    if (root != NULL)
        root->Render(m);
    render->model = mat4_identity;
}

UScene::~UScene(void)
{
    Free();
}
