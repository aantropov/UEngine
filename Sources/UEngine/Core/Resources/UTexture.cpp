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

bool UTexture:: Load(std:: string path)
{
    DEVIL_CALL(ilGenImages(1, &texture_res_id));
    DEVIL_CALL(ilBindImage(texture_res_id));

    DEVIL_CALL(if(ilLoadImage((LPCWSTR)path.c_str()) == false){return false;});
    //DEVIL_CALL(texture_id = ilutGLLoadImage((wchar_t*)path.c_str()));

    this->width  = ilGetInteger(IL_IMAGE_WIDTH);
    this->height = ilGetInteger(IL_IMAGE_HEIGHT);

    URenderer::GetInstance()->CreateTexture(this);

    DEVIL_CHECK_FOR_ERRORS();
    return true;
}

void UTexture::   Free()
{
    if(_id != -1)
        URenderer::GetInstance()->DeleteTexture(this);
    if(texture_res_id != 0)
        ilDeleteImages ( 1, &texture_res_id );
}
