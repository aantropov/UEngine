#include "UTexture.h"
#include "..\UEngine.h"

UTexture::UTexture(void)
{
    texture_res_id = 0;
}

UTexture::~UTexture(void)
{
    Free();
}

bool UTexture::Load(UXMLFile& xml, std::string path)
{
    this->name = xml.GetElement(path + "texture/name/");

    if (xml.isExistElement(path + "texture/wrap/"))
        wrap = xml.GetElement(path + "texture/wrap/") == "repeat" ? UTEXTURE_WRAP_REPEAT : UTEXTURE_WRAP_CLAMP;

    if (xml.isExistElement(path + "texture/filter/"))
        filter = xml.GetElement(path + "texture/filter/") == "linear" ? UTEXTURE_FILTER_LINEAR : UTEXTURE_FILTER_NEAREST;

    if (xml.isExistElement(path + "texture/mipmap/"))
        mipmap = atoi(xml.GetElement(path + "texture/mipmap/").c_str()) == 1;

    DEVIL_CALL(ilGenImages(1, &texture_res_id));
    DEVIL_CALL(ilBindImage(texture_res_id));

    DEVIL_CALL(if (ilLoadImage((LPCWSTR)xml.GetElement(path + "texture/path/").c_str()) == false) { return false; });
    //DEVIL_CALL(texture_id = ilutGLLoadImage((wchar_t*)path.c_str()));

    this->width = ilGetInteger(IL_IMAGE_WIDTH);
    this->height = ilGetInteger(IL_IMAGE_HEIGHT);
    this->image_format = ilGetInteger(IL_IMAGE_FORMAT);
    this->image_type = ilGetInteger(IL_IMAGE_TYPE);

    URenderer::GetInstance()->CreateTexture(this);

    if (texture_res_id != 0)
        ilDeleteImages(1, &texture_res_id);

    return true;
}

void UTexture::Free()
{
    if (_id != -1)
        URenderer::GetInstance()->DeleteTexture(this);
    if (texture_res_id != 0)
        ilDeleteImages(1, &texture_res_id);
}
