#pragma once
#include "..\Basic\UNode.h"
#include "UShader.h"
#include "..\Resources\UResourceFactory.h"

class UShader;

class UShaderProgram : public UNode, public UGLObject, public UResource
{
    UShader *fragment_sh;
    UShader *vertex_sh;

public:

    struct UUniformLocations
    {
        //unsigned int material_ambient;
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
        unsigned int transform_view;
        unsigned int transform_viewProjection;
        unsigned int transform_viewProjectionInv;
        unsigned int transform_normal;
        unsigned int transform_modelViewProjection;
        unsigned int transform_viewPosition; 
        
        unsigned int camera_znear;
        unsigned int camera_zfar;
        unsigned int camera_screen_size;
        unsigned int lightsNum;

        unsigned int skinning_transformsNum;
        unsigned int skinning_transforms;

    };

    UUniformLocations locations;

    void InitLocations();

    virtual bool Load(UXMLFile &xml, string path) override;

    void Render();
    virtual void Free();

    UShaderProgram(const UShaderProgram& s) { this->fragment_sh = s.fragment_sh; this->vertex_sh = s.vertex_sh; this->_id = s._id; };
    UShaderProgram(void);
    virtual ~UShaderProgram(void);
};