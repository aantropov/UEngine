#pragma once
#include "UGLObject.h"

class UVertexArrayObject: public UGLObject
{
public:
    
    bool Initialize();
    void Free();
    UVertexArrayObject(void){ _id = -1; };
    virtual ~UVertexArrayObject(void){Free();};
};