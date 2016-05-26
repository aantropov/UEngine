#pragma once
#include "../UEngine/Core/UEngine.h"

class USkyboxComponent : public UComponent
{
public:

    virtual void Update(double delta)
    {
        UComponent::Update(delta);
        auto renderer = URenderer::GetInstance();

        this->gameObject->local.position = vec4(renderer->currentCamera.GetPosition());
    }
};
