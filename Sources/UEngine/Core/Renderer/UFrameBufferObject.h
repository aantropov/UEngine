#pragma once
#include "UGLObject.h"
#include "..\Utils\enum.h"

class UTexture;

class UFrameBufferObject : public UGLObject
{
public:

	bool Initialize();
	void BindTexture(const UTexture *tex, UFramebufferAttachment type) const;
	void UnbindTexture(UFramebufferAttachment type) const;

	UFrameBufferObject(void);
	virtual ~UFrameBufferObject(void);
};