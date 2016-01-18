#include "UShader.h"
#include "urenderer.h"
#include "..\Utils\utils.h"

void UShader::Create(USHADER_TYPE st, std::vector<std::string> defines)
{
    _id = URenderer::GetInstance()->CompileShader(&source, st, defines);
}

bool UShader::LoadFromFile(std::string path)
{
    FILE* fShd;
    fopen_s(&fShd, trim(path).c_str(), "rb");

    if (fShd == NULL)
    {
        ULogger::GetInstance()->Message("Shader: Can`t open file " + path, ULOG_MSG_ERROR);
        return false;
    }

    while (!feof(fShd))
    {
        char temp = '\0';
        fread(&temp, 1, 1, fShd);
        source.push_back(temp);
    }

    if (fShd != NULL)
    {
        fclose(fShd);
    }
    return true;
}

void UShader::Free()
{
    if (_id != -1)
        URenderer::GetInstance()->DeleteShader(this);
    _id = -1;
    source.clear();
}