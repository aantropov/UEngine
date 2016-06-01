#pragma once
#include "..\Basic\unode.h"
#include "..\math\UMath.h"
#include "..\Renderer\UShaderProgram.h"
#include "UTexture.h"
#include <map>

struct UUniformParam
{
    vec4 data;
    unsigned int data_size;

    UUniformParam() : data_size(0) { }

    UUniformParam(float param) : data_size(1)
    {
        data.x = param;
    }

    UUniformParam(vec2 param) : data_size(2)
    {
        data.x = param.x;
        data.y = param.y;
    }

    UUniformParam(vec3 param) : data_size(3)
    {
        data.x = param.x;
        data.y = param.y;
        data.z = param.z;
    }

    UUniformParam(vec4 param) : data_size(4)
    {
        data.x = param.x;
        data.y = param.y;
        data.z = param.z;
        data.w = param.w;
    }
};

class UMaterial : public UResource, public UNode
{
    vec4  diffuse;
    vec4  specular;
    vec4  emission;
    float shininess;

    UShaderProgram *shader_forward;
    UShaderProgram *shader_depth;
    UShaderProgram *shader_normal;
    UShaderProgram *shader_deffered;

    std::map<unsigned int, UTexture*> uniform_textures;
    std::map<unsigned int, UCubemap*> uniform_cubemaps;

    mat4* skinningTransforms;
    unsigned int skinningTransformsNum;

    std::string name;

    bool is_depth_write_enabled = true;
    bool is_shadow_caster = true;
    int queue = 200;

    UBlendMode blend_mode = UBlendMode::Opaque;
    UDepthFunc depth_func = UDepthFunc::LEqual;

public:

    std::map<std::string, UUniformParam> params;

    UMaterial();
    UMaterial(vec4 dif, vec4 spec, vec4 emi, float shin, UShaderProgram *_sp);
    UMaterial(vec4 dif, vec4 spec, vec4 emi, float shin);

    void SetDepthFunc(UDepthFunc value) { depth_func = value; }
    UDepthFunc GetDepthFunc() const { return depth_func; }

    void SetBlendMode(UBlendMode value) { blend_mode = value; }
    UBlendMode GetBlendMode() const { return blend_mode; }

    bool IsShadowCaster() const { return is_shadow_caster; }
    void IsShadowCaster(bool value) { is_shadow_caster = value; }

    int GetQueue() const { return queue; }
    void SetQueue(unsigned int value) { queue = value; }

    void SetSkinningMatrixes(mat4 *skinningMatrixes, unsigned int num) { this->skinningTransforms = skinningMatrixes; this->skinningTransformsNum = num; }

    UShaderProgram* GetShaderProgram(URenderPass type);
    void SetShaderProgram(UShaderProgram *_sp, URenderPass type);

    void AddUniformUnit(unsigned int channel, UTexture* tex) { uniform_textures[channel] = tex; }
    void AddUniformUnit(unsigned int channel, UCubemap* tex) { uniform_cubemaps[channel] = tex; }

    void UnbindUniformUnits() const;
    void ClearUniformUnits() { uniform_textures.clear(); uniform_cubemaps.clear(); }

    //Setup material parameters in Shader
    virtual void Render(URenderPass type, int light_index = -1);

    void Render(UShaderProgram* sp);
    ~UMaterial(void) {}

    virtual bool Load(UXMLFile &xml, std::string path);
    virtual void Free() {}
};