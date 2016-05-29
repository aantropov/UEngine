#include "UCubemap.h"
#include "..\UEngine.h"

const std::string UCubemap::textures[6] = { "positive_x", "negative_x", "positive_y", "negative_y", "positive_z", "negative_z" };

UCubemap::UCubemap(void)
{
}

UCubemap::~UCubemap(void)
{
    Free();
}

void UCubemap::FreeHelperTextures()
{
    for (int i = 0; i < 6; i++)
        if (texture_res_id[i] != 0)
            ilDeleteImages(1, &texture_res_id[i]);
}

bool UCubemap::Load(UXMLFile& xml, std::string path)
{
    try
    {
        this->name = xml.GetElement(path + "cubemap/name/");

        if (xml.isExistElement(path + "cubemap/wrap/"))
            wrap = xml.GetElement(path + "cubemap/wrap/") == "repeat" ? UTextureWrapMode::Repeat : UTextureWrapMode::Clamp;

        if (xml.isExistElement(path + "cubemap/filter/"))
            filter = xml.GetElement(path + "cubemap/filter/") == "linear" ? UTextureFiltration::Linear : UTextureFiltration::Nearest;

        if (xml.isExistElement(path + "cubemap/mipmap/"))
            mipmap = atoi(xml.GetElement(path + "cubemap/mipmap/").c_str()) == 1;

        for (int i = 0; i < 6; i++)
        {
            DEVIL_CALL(ilGenImages(1, &texture_res_id[i]));
            DEVIL_CALL(ilBindImage(texture_res_id[i]));

            DEVIL_CALL(if (ilLoadImage((LPCWSTR)xml.GetElement(path + "cubemap/" + textures[i] + "/").c_str()) == false) { return false; });

            this->width = ilGetInteger(IL_IMAGE_WIDTH);
            this->height = ilGetInteger(IL_IMAGE_HEIGHT);
            this->image_format = ilGetInteger(IL_IMAGE_FORMAT);
            this->image_type = ilGetInteger(IL_IMAGE_TYPE);
        }
    }
    catch (exception e)
    {
        ULogger::GetInstance()->Message("Error to load cubemap (xml): " + path, ULogType::Error, ULogTarget::MsgBox);
        return false;
    }

    URenderer::GetInstance()->CreateCubemap(this);
    FreeHelperTextures();

    return true;
}

void UCubemap::Free()
{
    if (_id != -1)
        URenderer::GetInstance()->DeleteCubemap(this);

    FreeHelperTextures();
}
