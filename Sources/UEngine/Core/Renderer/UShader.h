#pragma once
#include "..\Resources\UResource.h"
#include "..\Utils\enum.h"
#include "UGLObject.h"

class UShader : public UResource, public UGLObject
{
    std::string source;

public:

    void Create(USHADER_TYPE st);
    virtual bool Load(std::string path);
    void Free();
    UShader(void){}
    virtual ~UShader(void) { Free(); }
};