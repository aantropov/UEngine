#pragma once
#include "..\Utils\ExportDef.h"
#include "UIUpdateble.h"

class UNode : public UIUpdateble
{
public:

    UENGINE_DECLSPEC UNode();
    virtual UENGINE_DECLSPEC void Update(double delta) {};

    virtual UENGINE_DECLSPEC void Free() {};
    virtual UENGINE_DECLSPEC ~UNode();
};