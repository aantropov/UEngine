#pragma once
#include "..\Utils\enum.h"
#include "UVertexBuffer.h"
#include "UIndexBuffer.h"
#include "UShaderProgram.h"
#include "URenderer.h"
#include "..\Resources\UMaterial.h"
#include "..\Resources\UTexture.h"
#include "..\Basic\ULight.h"
#include "..\Basic\UMatrix.h"
#include "..\Basic\UIRenderable.h"

class URenderQueue;

class UMesh : public UNode, public UIRenderable
{
    class UShaderLocations
    {
    public:

        int position;
        int normal;
        int texture_coord;
        int binormal;
        int bone_indices;
        int bone_weights;
    };

    bool InitializeBuffers()
    {
        return ib.Initialize() & vb.Initialize(&ib);
    }

    map<URenderPass, UShaderLocations> locations;
    sphere boundSphere;

public:

    std::string name;
    UVertexBuffer vb;
    UIndexBuffer ib;
    UMaterial material;

    void InitializeMaterial(URenderPass type);
    virtual void Initialize();

    virtual void AddToRenderQueue(URenderQueue& render_queue);
    virtual void Render(URenderPass type, int light_index = 0);

    sphere GetBounds() const;
    void ComputeBoundSphere();

    void Free() {}

    UMesh(void);
    ~UMesh(void);
};

