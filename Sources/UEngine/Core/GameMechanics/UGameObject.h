#pragma once
#include "UComponent.h"
#include "..\Basic\UTransform.h"

class UMaterial;

class UGameObject : public UNode{
	
public:
	
	UTransform transform;
	UTransform world;
	
	std::vector<UComponent*> components;

	std::vector<UGameObject*> children;
	UGameObject *parentObject;

	void UENGINE_DECLSPEC Render(UMaterial *m);
	void UENGINE_DECLSPEC Render(URENDER_TYPE type);
	virtual void UENGINE_DECLSPEC Update(double delta);

	UENGINE_DECLSPEC UGameObject();
	UENGINE_DECLSPEC UGameObject(UComponent *component);
	UENGINE_DECLSPEC ~UGameObject();	
};

