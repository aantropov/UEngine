#pragma once
#include "UFrameBufferObject.h"
#include "..\Resources\UPostEffect.h"
#include "..\Resources\UTexture.h"

class UScene;
class UCamera;
class URenderQueue;

class ULighting
{
public:

	UTexture* color;
	UTexture* depth;
	UTexture* normal;

	UFrameBufferObject fb;
	UFrameBufferObject post_effect_fb;

	virtual UTexture *Render(const UScene *scene, const UCamera camera, const URenderQueue& render_queue) = 0;
	friend class URenderManager;
};

class UForwardLightingOpaque : public ULighting
{
public:

	virtual  UTexture *Render(const UScene *scene, const UCamera camera, const URenderQueue& render_queue);
	UForwardLightingOpaque();
};

class UDefferedLightingOpaque : public ULighting
{
public:

	UTexture* diffuse;
	UTexture* specular;
	UTexture* position;

	UTexture* res_color_ping;
	UTexture* res_color_pong;

	UPostEffect *post_effect;

	virtual  UTexture *Render(const UScene *scene, const UCamera camera, const URenderQueue& render_queue);
	UDefferedLightingOpaque();
};

class UForwardLightingTranslucent : public ULighting
{
public:

	virtual  UTexture *Render(const UScene *scene, const UCamera camera, const URenderQueue& render_queue);
	UForwardLightingTranslucent(const UTexture* _depth);
};
