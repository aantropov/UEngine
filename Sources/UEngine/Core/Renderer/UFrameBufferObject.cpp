#include "UFrameBufferObject.h"
#include "URenderer.h"
#include "..\Resources\UTexture.h"

UFrameBufferObject::UFrameBufferObject(void)
{
}

UFrameBufferObject::~UFrameBufferObject(void)
{
}

bool UFrameBufferObject::Initialize()
{
    _id = -1;    
    _id = URenderer::GetInstance()->CreateFBO();
    URenderer::GetInstance()->UnbindFBO();

    return (_id != -1);
}

void UFrameBufferObject::BindTexture(UTexture *tex, UFRAMEBUFFER_ATTACHMENT type)
{
    URenderer::GetInstance()->BindFBO(this);
    glFramebufferTexture(GL_FRAMEBUFFER, type, tex->GetId(), 0);
}
