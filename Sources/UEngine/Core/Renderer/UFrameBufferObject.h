#pragma once
#include "UGLObject.h"
#include "..\Utils\enum.h"

class UTexture;

class UFrameBufferObject : public UGLObject
{
public:

    bool Initialize();
    void BindTexture(UTexture *tex, UFRAMEBUFFER_ATTACHMENT type);

    UFrameBufferObject(void);
    virtual ~UFrameBufferObject(void);
};