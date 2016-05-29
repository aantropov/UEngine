#pragma once
#include "..\GameMechanics\UGameMechanics.h"
#include "..\Renderer\UMesh.h"
#include "..\Basic\UIAnimatable.h"
#include "..\Utils\enum.h"

class URenderQueue;

class UModel : public UResource, public UComponent, public UIRenderable, public UISkinAnimatable
{
    unsigned int mesh_num;
    std::vector<UMesh*> meshes;

public:

    virtual void AddToRenderQueue(URenderQueue& render_queue);
    virtual void Render(URenderPass type, int light_index = 0);
    virtual void Update(double delta);

    virtual void Free();
    virtual bool Load(UXMLFile& xml, std::string path);
    virtual void UpdateAnimation(double delta) { UISkinAnimatable::UpdateAnimation(delta); }

    sphere GetBounds() const
    {
        float max_radius = 0.0f;
        for each(auto mesh in meshes)
        {
            if (max_radius < mesh->GetBounds().radius)
                max_radius = mesh->GetBounds().radius;
        }

        return sphere(vec3_zero, max_radius);
    }

    UModel(void) {}
    virtual ~UModel(void) {}
};

