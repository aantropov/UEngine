#pragma once
#include "..\Renderer\umesh.h"

class UPostEffect : public UResource, public UMesh
{

public:

    void ClearUniformUnits();
    void AddTexture(UTexture* tex, int channel);

    virtual void Render(URENDER_TYPE type, int lightIndex = 0);
    virtual void Update(double delta);
    virtual void Free();
    virtual bool Load(UXMLFile &xml, std:: string path);

    UPostEffect(void);
    virtual ~UPostEffect(void);
};

