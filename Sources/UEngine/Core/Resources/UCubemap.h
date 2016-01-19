#pragma once
#include "uresource.h"
#include "..\Renderer\urenderer.h"
#include "..\Renderer\UGLObject.h"

class UCubemap : public UResource, public UGLObject
{
    static const std::string textures[6];
    ILuint texture_res_id[6];

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

    ILuint* GetHelperTextures() { return texture_res_id; }
    void FreeHelperTextures();

    bool GetMipMap() const { return mipmap; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    int GetImageType() const { return image_type; }
    int GetImageFormat() const { return image_format; }
    UTEXTURE_FILTER GetImageFilter() const { return filter; }
    UTEXTURE_WRAP GetImageWrap() const { return wrap; }
    UTEXTURE_TYPE GetType() const { return type; }

    void GenTexture() { glGenTextures(1, (GLuint*)&_id); }

    void Create()
    {
        _id = URenderer::GetInstance()->CreateCubemap(this);
    }

    virtual void Free();
    virtual bool Load(UXMLFile &xml, std::string path);

    UCubemap(void);
    virtual ~UCubemap(void);
};