#pragma once

#include "UComponent.h"
#include "..\Math\UMath.h"

class UMaterial;
class UMesh;
class URenderQueue;

class UGameObject : public UNode
{
protected:
    std::vector<UComponent*> components;

public:

    std::string name;

    ::transform local_transform;
    ::transform parent_transform;

	UGameObject *parent_object;
    std::vector<UGameObject*> children;

    void UENGINE_DECLSPEC GetBounds() const;
    void UENGINE_DECLSPEC AddComponent(UComponent *component);

    void UENGINE_DECLSPEC AddToRenderQueue(URenderQueue& render_queue);
    virtual void UENGINE_DECLSPEC Update(double delta);

    UENGINE_DECLSPEC UGameObject();
    UENGINE_DECLSPEC UGameObject(UComponent *component);
    UENGINE_DECLSPEC ~UGameObject();
};

