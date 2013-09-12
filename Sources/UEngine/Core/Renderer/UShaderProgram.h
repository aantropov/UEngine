#pragma once
#include "..\Basic\UNode.h"
#include "UShader.h"
#include "..\Resources\UResourceFactory.h"

class UShader;

class UShaderProgram : public UNode, public UGLObject, public UResource
{    
    UShader *pixel_sh;
    UShader *vertex_sh;

public:

    struct UUniformLocations
    {
        unsigned int material_ambient;
        unsigned int material_diffuse;
        unsigned int material_specular;
        unsigned int material_emission;
        unsigned int material_shininess;

        unsigned int light_position;
        unsigned int light_ambient;
        unsigned int light_diffuse;
        unsigned int light_specular;
        unsigned int light_attenuation;
        unsigned int light_spotDirection;
        unsigned int light_spotExponent;
        unsigned int light_spotCosCutoff;
        unsigned int light_transform;
        unsigned int light_depthTextures;

        unsigned int transform_model;
        unsigned int transform_viewProjection;
        unsigned int transform_normal;
        unsigned int transform_modelViewProjection;
        unsigned int transform_viewPosition;

        unsigned int lightsNum;
        
        unsigned int skinning_transformsNum;
        unsigned int skinning_transforms;
        
    };

    UUniformLocations locations;

    void InitLocations();

    virtual bool Load(string path);
    bool Load(std::string vertexshd_path, std::string pixelshd_path);

    void Render();
    virtual void Free();

    UShaderProgram(const UShaderProgram& s) {this->pixel_sh = s.pixel_sh; this->vertex_sh = s.vertex_sh; this->_id = s._id; };
    UShaderProgram(void);
    virtual ~UShaderProgram(void);
};