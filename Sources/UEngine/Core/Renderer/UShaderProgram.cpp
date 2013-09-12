#include "UShaderProgram.h"
#include "UShader.h"
#include "..\Resources\UResourceFactory.h"
#include "..\UEngine.h"

void UShaderProgram::Free()
{    
        if(_id != -1)
            URenderer::GetInstance()->DeleteShaderProgram(this);
        _id = -1;
}

void UShaderProgram:: Render()
{
    URenderer::GetInstance()->SetShaderProgram(this);
}

void UShaderProgram:: InitLocations()
{
    locations.material_ambient = glGetUniformLocation(_id, "material.ambient" );
    locations.material_diffuse = glGetUniformLocation(_id, "material.diffuse" );
    locations.material_specular = glGetUniformLocation(_id, "material.specular" );
    locations.material_emission = glGetUniformLocation(_id, "material.emission" );
    locations.material_shininess = glGetUniformLocation(_id, "material.shininess" );

    locations.light_position = glGetUniformLocation(_id, "light_position" );
    locations.light_ambient = glGetUniformLocation(_id, "light_ambient" );
    locations.light_diffuse = glGetUniformLocation(_id, "light_diffuse" );
    locations.light_specular = glGetUniformLocation(_id, "light_specular" );
    locations.light_attenuation = glGetUniformLocation(_id, "light_attenuation" );
    locations.light_spotDirection = glGetUniformLocation(_id, "light_spotDirection" );
    locations.light_spotExponent = glGetUniformLocation(_id, "light_spotExponent" );
    locations.light_spotCosCutoff = glGetUniformLocation(_id, "light_spotCosCutoff" );
    locations.light_transform = glGetUniformLocation(_id, "light_transform" );
    locations.light_depthTextures = glGetUniformLocation(_id, "light_depthTexture");

    locations.transform_model = glGetUniformLocation(_id, "transform.model" );
    locations.transform_viewProjection = glGetUniformLocation(_id, "transform.viewProjection" );
    locations.transform_normal = glGetUniformLocation(_id, "transform.normal" );
    locations.transform_modelViewProjection = glGetUniformLocation(_id, "transform.modelViewProjection" );
    locations.transform_viewPosition = glGetUniformLocation(_id, "transform.viewPosition" );
    locations.lightsNum = glGetUniformLocation(_id, "lightsNum" );

    locations.skinning_transformsNum = glGetUniformLocation(_id, "skinning_transformsNum" );
    locations.skinning_transforms = glGetUniformLocation(_id, "skinning_transforms" );
}

bool UShaderProgram:: Load(string path)
{
    //not implemented yet
    return false;
}

bool UShaderProgram:: Load(std::string vertexshd_path, std::string pixelshd_path)
{
    UShader *vs = dynamic_cast<UShader*>(rf->Get(vertexshd_path));
    UShader *ps = dynamic_cast<UShader*>(rf->Get(pixelshd_path));

    vertex_sh = dynamic_cast<UShader*> (rf->Load(vertexshd_path, URESOURCE_SHADER));
    pixel_sh = dynamic_cast<UShader*> (rf->Load(pixelshd_path, URESOURCE_SHADER));

    if(vs == nullptr)
        vertex_sh->Create(USHADER_VERTEX);
    if(ps == nullptr)
        pixel_sh->Create(USHADER_PIXEL);

    _id = URenderer::GetInstance()->CreateShaderProgram(vertex_sh, pixel_sh);

    InitLocations();

    return true;
}

/*
void UShaderProgram:: CreateShaderProgram(std::string vertexshd_path, std::string pixelshd_path){
        
        vertex_sh.Load(vertexshd_path,&vertex_sh);
        pixel_sh.Load(pixelshd_path, &pixel_sh);
                
        vertex_sh.Create(USHADER_VERTEX);
        pixel_sh.Create(USHADER_PIXEL);

        shader_program_id = URenderer::GetInstance()->CreateShaderProgram(&vertex_sh, &pixel_sh);
}
/**/

UShaderProgram::UShaderProgram(void)
{
}

UShaderProgram::~UShaderProgram(void)
{
    Free();
}