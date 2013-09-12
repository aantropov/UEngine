#pragma once

#include "UComponent.h"
#include "..\Math\UMath.h"

class UMaterial;

class UGameObject : public UNode
{
protected:
    std::vector<UComponent*> components;

public:
    
    std::string name;

    ::transform local;
    ::transform world;
    
    std::vector<UGameObject*> children;
    UGameObject *parentObject;

    void UENGINE_DECLSPEC AddComponent(UComponent *component);

    void UENGINE_DECLSPEC Render(UMaterial *m);
    void UENGINE_DECLSPEC Render(URENDER_TYPE type);
    virtual void UENGINE_DECLSPEC Update(double delta);

    UENGINE_DECLSPEC UGameObject();
    UENGINE_DECLSPEC UGameObject(UComponent *component);
    UENGINE_DECLSPEC ~UGameObject();    
};

