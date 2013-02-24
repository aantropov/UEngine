#include "UGameObject.h"
#include "..\UEngine.h"

void UGameObject:: AddComponent(UComponent *component)
{
	components.push_back(component);
}

void UGameObject::Render(UMaterial *m)
{
	for each(auto el in components)
	{
		auto renderable = dynamic_cast<UIRenderable*>(el);
		if(renderable != NULL)
		{
			auto localTransform = (transform * world);
			renderable->m.Set(localTransform.ToMatrix());
			renderable->Render(m);
		}
	}	
}
	
void UGameObject::Render(URENDER_TYPE type)
{
	for each(auto el in components)
	{
		auto renderable = dynamic_cast<UIRenderable*>(el);
		if(renderable != NULL)
		{
			auto localTransform = (transform * world);
			renderable->m.Set(localTransform.ToMatrix());
			renderable->Render(type);
		}
	}	
}

void UGameObject:: Update(double delta)
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
	for(unsigned int i = 0; i < components.size(); i++)
	{
		if(dynamic_cast<UResource*>(components[i]) == nullptr)
			delete components[i];
	}
};
