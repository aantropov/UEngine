#pragma once
#include "..\Utils\ExportDef.h"
#include "UIUpdateble.h"

//UNode may be created and stored in scene hierarchy
class UNode : public UIUpdateble
{    
public:
    
    UENGINE_DECLSPEC UNode();
    virtual UENGINE_DECLSPEC void Update(double delta){};
    // Standard RAII
    virtual UENGINE_DECLSPEC void Free(){};
    virtual UENGINE_DECLSPEC ~UNode();
};