#include "UFrameBufferObject.h"
#include "URenderer.h"
#include "..\Resources\UTexture.h"

UFrameBufferObject::UFrameBufferObject(void)
{
}

UFrameBufferObject::~UFrameBufferObject(void)
{
    if (_id != -1)
        glDeleteFramebuffers(1, (GLuint*)&_id);
}

bool UFrameBufferObject::Initialize()
{
    _id = -1;
    _id = URenderer::GetInstance()->CreateFBO();
    URenderer::GetInstance()->UnbindFBO();

    return (_id != -1);
}

void UFrameBufferObject::BindTexture(const UTexture *tex, UFramebufferAttachment type) const
{
    URenderer::GetInstance()->BindFBO(this);
    glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)type, GL_TEXTURE_2D, tex->GetId(), 0);
}

void UFrameBufferObject::UnbindTexture(UFramebufferAttachment type) const
{
    URenderer::GetInstance()->BindFBO(this);
    glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)type, GL_TEXTURE_2D, 0, 0);
}
