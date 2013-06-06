#pragma once
#include "uresource.h"
#include "..\Renderer\urenderer.h"
#include "..\Renderer\UGLObject.h"

// Texture
class UTexture :
    public UResource, public UGLObject
{
    //Il id
    unsigned int texture_res_id;

    //Parameters
    int width, height;

    //Type of texture
    UTEXTURE_TYPE type;
        
public:

    std::string name;

    unsigned int GetTextureResId() {return texture_res_id;}

    int GetWidth() {return width;}
    int GetHeight() {return height;}
    UTEXTURE_TYPE GetType() {return type;}
    
    //Setters
    void SetWidth(int w) { width = w; }
    void SetHeight(int h) { height = h; }
    void SetType(UTEXTURE_TYPE type) { this->type = type; }

    void GenTexture(){ glGenTextures(1, (GLuint*)&_id);}

    //Create texture
    void Create(int w, int h, UTEXTURE_TYPE t){
        width = w;
        height = h;
        type = t;
        _id = URenderer::GetInstance()->CreateTexture(this);
    }
        
    virtual void Free();
    virtual bool Load(std:: string path);

    UTexture(void);
    virtual ~UTexture(void);
};

