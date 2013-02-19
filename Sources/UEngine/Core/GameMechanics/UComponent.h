#pragma once
#include "..\Basic\UNode.h"
#include "..\Basic\UIAnimable.h"

class UGameObject;

class UComponent : public UNode {

public:

	UGameObject *gameObject;

	virtual void UENGINE_DECLSPEC Update(double delta)
	{
		auto anim = dynamic_cast<UIAnimable*>(this);
		if(anim != nullptr)
			anim->UpdateAnimation(delta);
	}

	UENGINE_DECLSPEC UComponent(){}
	virtual UENGINE_DECLSPEC ~UComponent(){}
};

