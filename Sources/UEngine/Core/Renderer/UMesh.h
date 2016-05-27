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

    map<URENDER_PASS, UShaderLocations> locations;
    sphere boundSphere;

public:

    UMatrix m;

    // Data    
    std::string name;
    UVertexBuffer vb;
    UIndexBuffer ib;
    UMaterial material;

    void InitializeMaterial(URENDER_PASS type);
    virtual void Initialize();

    virtual void AddToRenderQueue(map<int, vector<UMesh*>>& render_queue);
    virtual void Render(UMaterial *mat);
    virtual void Render(URENDER_PASS type, int light_index = 0);

    sphere GetBounds() const;
    void ComputeBoundSphere();

    void Free() {}

    UMesh(void);
    ~UMesh(void);
};

