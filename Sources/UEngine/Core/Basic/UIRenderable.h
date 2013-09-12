#pragma once
#include "UMatrix.h"
#include "..\Utils\enum.h"

class UMaterial;

class UIRenderable
{
protected:
public:
    
    UMatrix m;

    virtual void Render(URENDER_TYPE type) = 0;
    virtual void Render(UMaterial* m) = 0;

    UIRenderable(void);
    virtual ~UIRenderable(void);
};