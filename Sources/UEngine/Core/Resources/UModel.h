#pragma once
#include "..\GameMechanics\UGameMechanics.h"
#include "..\Renderer\UMesh.h"
#include "..\Basic\UIAnimatable.h"

class UModel: public UResource, public UComponent, public UIRenderable, public UISkinAnimatable
{
    unsigned int mesh_num;
    std::vector<UMesh*> meshes;    
    
public:
    
    virtual void Render(URENDER_TYPE type);
    virtual void Render(UMaterial *mat);
    virtual void Update(double delta);
    
    virtual void Free();
    virtual bool Load(std:: string path);
    virtual void UpdateAnimation(double delta) { UISkinAnimatable::UpdateAnimation(delta); }
    UModel(void){}
    virtual ~UModel(void){}
};

