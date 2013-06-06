#pragma once
#include "..\Resources\UResource.h"
#include "..\Utils\enum.h"
#include "UGLObject.h"

class UShader :
    public UResource, public UGLObject
{
    // Source code
    std::string source;

public:

/*    void SetParam(string location, unsigned int n, vec4 *v);
    void SetParam(string location, unsigned int n, vec3 *v);
    void SetParam(string location, unsigned int n, mat4 *v);
    void SetParam(string location, unsigned int n, mat3 *v);
    void SetParam(string location, unsigned int n, float *v);
    */
    //Compile Shader
    void Create(USHADER_TYPE st);

    //Load from file source code of the shader
    virtual bool Load(std::string path);

    //Delete all data
    void Free();

    UShader(void){}
    virtual ~UShader(void) { Free(); }
};

