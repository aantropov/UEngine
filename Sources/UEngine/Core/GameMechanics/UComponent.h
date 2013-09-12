#pragma once
#include "..\Basic\UNode.h"
#include "..\Basic\UIAnimatable.h"

class UGameObject;

class UComponent : public UNode 
{
public:

    std::string name;
    UGameObject *gameObject;

    virtual void UENGINE_DECLSPEC Update(double delta)
    {
        auto anim = dynamic_cast<UIAnimatable*>(this);
        if(anim != nullptr)
            anim->UpdateAnimation(delta);
    }

    UENGINE_DECLSPEC UComponent(){}
    virtual UENGINE_DECLSPEC ~UComponent(){}
};

