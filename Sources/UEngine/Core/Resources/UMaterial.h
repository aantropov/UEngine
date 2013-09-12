#pragma once
#include "..\Basic\unode.h"
#include "..\math\UMath.h"
#include "..\Renderer\UShaderProgram.h"
#include "UTexture.h"
#include <map>

class UMaterial : public UNode, public UResource
{
    vec4  ambient;
    vec4  diffuse;
    vec4  specular;
    vec4  emission;
    float shininess;

    UShaderProgram *spForward;
    UShaderProgram *spDepth;
    UShaderProgram *spNormal;
    UShaderProgram *spDeffered;

    std::vector<std::pair<UTexture*, unsigned int>> textures;

    mat4* skinningTransforms;
    unsigned int skinningTransformsNum;

    std::string name;

public:
    
    std::map<std::string, float> params;

    UMaterial();
    UMaterial(vec4 amb, vec4 dif, vec4 spec, vec4 emi, float shin, UShaderProgram *_sp);
    UMaterial(vec4 amb, vec4 dif, vec4 spec, vec4 emi, float shin);
    
    void SetSkinningMatrixes(mat4 *skinningMatrixes, unsigned int num) { this->skinningTransforms = skinningMatrixes; this->skinningTransformsNum = num; }

    UShaderProgram* GetShaderProgram(URENDER_TYPE type);
    void SetShaderProgram(UShaderProgram *_sp, URENDER_TYPE type);

    void AddTexture(pair<UTexture*, unsigned int> tex) { textures.push_back(tex); }
    void ClearTextures() { textures.clear(); }

    //Setup material parameters in Shader
    virtual void Render(URENDER_TYPE type);
    void Render(UShaderProgram* sp);
    ~UMaterial(void) {}


    virtual bool Load(std::string path);
    virtual void Free() {}
};