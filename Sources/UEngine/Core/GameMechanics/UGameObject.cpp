#include "UGameObject.h"
#include "..\UEngine.h"

void UGameObject::AddComponent(UComponent *component)
{
    component->gameObject = this;
    components.push_back(component);
}

void UGameObject::Render(UMaterial *m)
{
    auto render = URenderer::GetInstance();
    for each(auto el in components)
    {
        auto renderable = dynamic_cast<UIRenderable*>(el);
        if (renderable != NULL)
        {
            auto currentTransform = (local_transform * parent_transform);
            renderable->m.Set(currentTransform.matrix());

            auto model = dynamic_cast<UModel*>(el);
            if (model != nullptr)
            {
                auto sph = model->GetBounds();
                sph.center = currentTransform * sph.center;

                if (IsSphereInFrustum(sph, render->current_camera.GetFrustum()))
                    renderable->Render(m);
            }
            else
                renderable->Render(m);
        }
    }
}

void UGameObject::Render(URENDER_PASS type)
{
    auto render = URenderer::GetInstance();
    for each(auto el in components)
    {
        auto renderable = dynamic_cast<UIRenderable*>(el);
        if (renderable != NULL)
        {
            auto currentTransform = (local_transform * parent_transform);
            renderable->m.Set(currentTransform.matrix());

            auto model = dynamic_cast<UModel*>(el);
            if (model != nullptr)
            {
                auto sph = model->GetBounds();
                sph.center = currentTransform * sph.center;

                if (IsSphereInFrustum(sph, render->current_camera.GetFrustum()))
                    renderable->Render(type);
            }
            else
                renderable->Render(type);
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
