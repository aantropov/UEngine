#include "UScene.h"
#include "..\Renderer\URenderer.h"
#include "ULight.h"
#include "..\Renderer\UMesh.h"

void UScene::USceneNode::AddChild(USceneNode* n)
{
    node->children.push_back(n->node);
    children.push_back(n);
}

void UScene::USceneNode::AddToRenderQueue(map<int, list<pair<mat4, UMesh*>>>& render_queue)
{
    auto render = URenderer::GetInstance();
    render->PushModelMatrix();

    auto currentTransform = node->local_transform * node->parent_transform;
    render->model_view *= currentTransform.matrix();

    node->AddToRenderQueue(render_queue);
    for (unsigned int i = 0; i < children.size(); i++) {
        if (children[i] != NULL)
            children[i]->AddToRenderQueue(render_queue);
    }

    render->PopModelMatrix();
}

void UScene::USceneNode::Update(double delta)
{
    if (node != NULL && !is_already_updated)
    {
        node->Update(delta);
        is_already_updated = true;
    }

    for (unsigned int i = 0; i < children.size(); i++)
    {
        children[i]->node->parent_object = node;
        children[i]->node->parent_transform = node->local_transform * node->parent_transform;
        children[i]->Update(delta);
    }

    is_already_updated = false;
}

void UScene::USceneNode::DeleteTopology()
{
    for (unsigned int i = 0; i < children.size(); i++) {
        USceneNode *temp = NULL;
        temp = dynamic_cast<USceneNode*>(children[i]);

        if (temp != NULL) {
            temp->DeleteTopology();
            delete temp;
        }
    }
}

void UScene::USceneNode::Free()
{
    children.clear();
}

UScene::USceneNode:: ~USceneNode()
{
    Free();
}

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

/*
void UScene::Render(URENDER_PASS type, UCamera camera)
{
    auto render = URenderer::GetInstance();

    render->SetCurrentCamera(camera);
    render->current_camera.UpdateFrustum();

    if (root != NULL)
        root->Render(type);

    render->model_view = mat4_identity;
}
*/

void UScene::PrepareRenderQueue()
{
    auto render = URenderer::GetInstance();
    render->model_view = mat4_identity;

    render_queue.clear();
    root->AddToRenderQueue(render_queue);
}

void UScene::RenderQueue(URENDER_PASS type, UCamera camera)
{
    vector<int> keys;
    for (auto it = render_queue.begin(); it != render_queue.end(); ++it)
        keys.push_back(it->first);

    keys.erase(unique(keys.begin(), keys.end()), keys.end());

    std::sort(keys.begin(), keys.end());

    auto render = URenderer::GetInstance();

    render->SetCurrentCamera(camera);
    render->current_camera.UpdateFrustum();

    for (auto it = keys.begin(); it != keys.end(); ++it)
    {
        auto vec = render_queue[*it];
        for (auto mesh_it = vec.begin(); mesh_it != vec.end(); ++mesh_it)
        {
            render->model_view = (*mesh_it).first;
            (*mesh_it).second->Render(type);
        }
    }

    render->model_view = mat4_identity;
}

UScene::~UScene(void)
{
    Free();
}
