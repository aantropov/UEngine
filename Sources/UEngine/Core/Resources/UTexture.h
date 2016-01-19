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

    UTEXTURE_TYPE type;
    UTEXTURE_FILTER filter;
    UTEXTURE_WRAP wrap;

public:

    std::string name;

    unsigned int GetTextureResId() { return texture_res_id; }
    
    bool GetMipMap() const { return mipmap; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    int GetImageType() const { return image_type; }
    int GetImageFormat() const { return image_format; }
    UTEXTURE_FILTER GetImageFilter() const { return filter; }
    UTEXTURE_WRAP GetImageWrap() const { return wrap; }
    UTEXTURE_TYPE GetType() const { return type; }

    void SetType(UTEXTURE_TYPE type) { this->type = type; }
    void GenTexture() { glGenTextures(1, (GLuint*)&_id); }

    void Create(int w, int h, UTEXTURE_TYPE t, UTEXTURE_FILTER filt = UTEXTURE_FILTER_LINEAR, UTEXTURE_WRAP wr = UTEXTURE_WRAP_CLAMP)
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