#pragma once
#include "UGLObject.h"

class UBuffer: public UGLObject
{
public:

    virtual void* Lock() = 0;
    virtual void Unlock() = 0;

    virtual void* GetPointer() = 0;
    virtual int GetNum() = 0;
    virtual void Create(int num) = 0;
    virtual bool Initialize() = 0;
    virtual void Free() = 0;
    
    UBuffer(void);
    virtual ~UBuffer(void);
};