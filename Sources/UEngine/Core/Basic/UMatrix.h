#pragma once
#include "unode.h"
#include "..\math\umath.h"

class UMatrix : public UNode
{
protected:
    mat4 m;

public:    
    
    void Set(mat4 m1);

    mat4 Get();
    void Set();

    virtual void Render();
    virtual void Update();

    UMatrix(void);
    ~UMatrix(void);
};