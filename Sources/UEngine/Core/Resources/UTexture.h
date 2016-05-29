#pragma once
#include "uresource.h"
#include "..\Renderer\urenderer.h"
#include "..\Renderer\UGLObject.h"

class UTexture : public UResource, public UGLObject
{
    unsigned int texture_res_id;

    int width;
    int height;
    int image_format;
    int image_type;
    bool mipmap;

    UTextureFormat type;
    UTextureFiltration filter;
    UTextureWrapMode wrap;

public:

    std::string name;

    unsigned int GetTextureResId() { return texture_res_id; }

    bool GetMipMap() const { return mipmap; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    int GetImageType() const { return image_type; }
    int GetImageFormat() const { return image_format; }
    UTextureFiltration GetImageFilter() const { return filter; }
    UTextureWrapMode GetImageWrap() const { return wrap; }
    UTextureFormat GetFormat() const { return type; }

    void SetType(UTextureFormat type) { this->type = type; }
    void SetMipMap(bool mipmapping) { mipmap = mipmapping; }
    void GenTexture() { glGenTextures(1, (GLuint*)&_id); }

    void Create(int w, int h, UTextureFormat t, UTextureFiltration filt = UTextureFiltration::Linear, UTextureWrapMode wr = UTextureWrapMode::Clamp)
    {
        width = w;
        height = h;
        type = t;
        filter = filt;
        wrap = wr;
        _id = URenderer::GetInstance()->CreateTexture(this);
    }

    virtual void Free();
    virtual bool Load(UXMLFile &xml, std::string path);

    UTexture(void);
    virtual ~UTexture(void);
};