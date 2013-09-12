#pragma once
#include "..\Renderer\umesh.h"

class UPostEffect : public UResource, public UMesh
{

public:

    void AddTexture(UTexture* tex, int channel);

    virtual void Render(URENDER_TYPE type);
    virtual void Update(double delta);
    virtual void Free();
    virtual bool Load(std:: string path);

    UPostEffect(void);
    virtual ~UPostEffect(void);
};

