#include "UGameObject.h"
#include "..\UEngine.h"
#include "..\Renderer\UMesh.h"
#include "..\Renderer\URenderManager.h"

void UGameObject::AddComponent(UComponent *component)
{
    component->gameObject = this;
    components.push_back(component);
}

void UGameObject::AddToRenderQueue(URenderQueue& render_queue)
{
    auto render = URenderer::GetInstance();
    for each(auto el in components)
    {
        auto renderable = dynamic_cast<UIRenderable*>(el);
        if (renderable != NULL)
        {
            auto model = dynamic_cast<UModel*>(el);
            if (model != nullptr)
            {
                auto sph = model->GetBounds();
                sph.center = render->model_view * sph.center;

                if (IsSphereInFrustum(sph, render->current_camera.GetFrustum()))
                    renderable->AddToRenderQueue(render_queue);
            }
            else
                renderable->AddToRenderQueue(render_queue);
        }
    }
}

void UGameObject::Update(double delta)
{
    for each(auto el in components)
        el->Update(delta);
}

UGameObject::UGameObject()
{
    //world = mat4_identity;
}

UGameObject::UGameObject(UComponent *component)
{
    UGameObject();
    component->gameObject = this;
    components.push_back(component);
}

UGameObject::~UGameObject()
{
    for (unsigned int i = 0; i < components.size(); i++)
    {
        if (dynamic_cast<UResource*>(components[i]) == nullptr)
            delete components[i];
    }
};
