#pragma once
#include "..\GameMechanics\UGameMechanics.h"
#include "..\Renderer\UMesh.h"
#include "..\Basic\UIAnimable.h"

class UModel: public UResource, public UComponent, public UIRenderable, public UISkinAnimable
{
	std::string name;
	unsigned int mesh_num;

	std::vector<UMesh*> meshes;	
	
public:
	
	virtual void Render(URENDER_TYPE type);
	virtual void Render(UMaterial *mat);
	virtual void Update(double delta);
	
	virtual void Free();
	virtual bool Load(std:: string path);
	virtual void UpdateAnimation(double delta) { UISkinAnimable::UpdateAnimation(delta); }
	UModel(void){}
	virtual ~UModel(void){}
};

