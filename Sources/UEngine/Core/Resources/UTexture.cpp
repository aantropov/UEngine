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
    xml.TryGetElement(path + "texture/name/", name);
    xml.TryGetElementEnum<UTextureWrapMode, UTextureWrapModeHelper>(path + "texture/wrap/", wrap);
    xml.TryGetElementEnum<UTextureFiltration, UTextureFiltrationHelper>(path + "texture/filter/", filter);
    xml.TryGetElementb(path + "texture/mipmap/", mipmap);

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
