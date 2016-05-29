#pragma once
#include "..\Resources\UResource.h"
#include "..\Utils\enum.h"
#include "UGLObject.h"

class UShader : public UResource, public UGLObject
{
    std::string source;

public:

    void Create(UShaderType st, std::vector<std::string> defines);
    virtual bool Load(UXMLFile& xml, std::string xmlPath) override { return false; };
    virtual bool LoadFromFile(std::string xmlPath) override;

    void Free();
    UShader(void) {}
    virtual ~UShader(void) { Free(); }
};