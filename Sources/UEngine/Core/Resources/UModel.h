#pragma once
#include "..\GameMechanics\UGameMechanics.h"
#include "..\Renderer\UMesh.h"
#include "..\Basic\UIAnimatable.h"

class UModel : public UResource, public UComponent, public UIRenderable, public UISkinAnimatable
{
    unsigned int mesh_num;
    std::vector<UMesh*> meshes;

public:

    virtual void Render(URENDER_PASS type, int light_index = 0);
    virtual void Render(UMaterial *mat);
    virtual void Update(double delta);

    virtual void Free();
    virtual bool Load(UXMLFile& xml, std::string path);
    virtual void UpdateAnimation(double delta) { UISkinAnimatable::UpdateAnimation(delta); }

    sphere GetBounds() const
    {
        float maxRadius = 0.0f;
        for each(auto mesh in meshes)
        {
            if (maxRadius < mesh->GetBounds().radius)
                maxRadius = mesh->GetBounds().radius;
        }

        return sphere(vec3_zero, maxRadius);
    }

    UModel(void) {}
    virtual ~UModel(void) {}
};

