#include "UShaderProgram.h"
#include "UShader.h"
#include "..\Resources\UResourceFactory.h"
#include "..\UEngine.h"
#include "..\Utils\utils.h"

void UShaderProgram::Free()
{
    if (_id != -1)
        URenderer::GetInstance()->DeleteShaderProgram(this);
    _id = -1;
}

void UShaderProgram::Render()
{
    URenderer::GetInstance()->SetShaderProgram(this);
}

void UShaderProgram::InitLocations()
{

    //locations.material_ambient = glGetUniformLocation(_id, "material.ambient");
    locations.material_diffuse = glGetUniformLocation(_id, "material.diffuse");
    locations.material_specular = glGetUniformLocation(_id, "material.specular");
    locations.material_emission = glGetUniformLocation(_id, "material.emission");
    locations.material_shininess = glGetUniformLocation(_id, "material.shininess");
    locations.light_position = glGetUniformLocation(_id, "light_position");
    locations.light_ambient = glGetUniformLocation(_id, "light_ambient");
    locations.light_diffuse = glGetUniformLocation(_id, "light_diffuse");
    locations.light_specular = glGetUniformLocation(_id, "light_specular");
    locations.light_attenuation = glGetUniformLocation(_id, "light_attenuation");
    locations.light_spotDirection = glGetUniformLocation(_id, "light_spotDirection");
    locations.light_spotExponent = glGetUniformLocation(_id, "light_spotExponent");
    locations.light_spotCosCutoff = glGetUniformLocation(_id, "light_spotCosCutoff");
    locations.light_transform = glGetUniformLocation(_id, "light_transform");
    locations.light_depthTextures = glGetUniformLocation(_id, "light_depthTexture");
    locations.lightsNum = glGetUniformLocation(_id, "lightsNum");

    locations.transform_model = glGetUniformLocation(_id, "transform.model");
    locations.transform_view = glGetUniformLocation(_id, "transform.view");
    locations.transform_viewProjection = glGetUniformLocation(_id, "transform.viewProjection");
    locations.transform_viewProjectionInv = glGetUniformLocation(_id, "transform.viewProjectionInv");
    locations.transform_normal = glGetUniformLocation(_id, "transform.normal");
    locations.transform_modelViewProjection = glGetUniformLocation(_id, "transform.modelViewProjection");
    locations.transform_viewPosition = glGetUniformLocation(_id, "transform.viewPosition");
    locations.camera_znear = glGetUniformLocation(_id, "camera.zNear");
    locations.camera_zfar = glGetUniformLocation(_id, "camera.zFar");
    locations.camera_screen_size = glGetUniformLocation(_id, "camera.screenSize");
    
    locations.skinning_transformsNum = glGetUniformLocation(_id, "skinning_transformsNum");
    locations.skinning_transforms = glGetUniformLocation(_id, "skinning_transforms");

    OPENGL_CHECK_FOR_ERRORS();
}

bool UShaderProgram::Load(UXMLFile& xml, std::string path)
{
    try
    {
        std::string defines;
        std::vector<std::string> defines_array;

        if (xml.isExistElement(path + "shader/defines/"))
        {
            defines = trim(xml.GetElement(path + "shader/defines/"));
            defines_array = split(defines);
        }

        std::string shader_path = xml.GetElement(path + "shader/path/");

        std::string vertex_shader_path(shader_path + " VERTEX " + defines);
        std::string fragment_shader_path(shader_path + " FRAGMENT " + defines);

        vertex_sh = dynamic_cast<UShader*>(rf->Get(vertex_shader_path));
        fragment_sh = dynamic_cast<UShader*>(rf->Get(fragment_shader_path));

        if (vertex_sh == nullptr)
        {
            vertex_sh = (UShader*)rf->Create(vertex_shader_path, URESOURCE_SHADER);
            vertex_sh->LoadFromFile(shader_path);
            vertex_sh->Create(USHADER_VERTEX, defines_array);
        }

        if (fragment_sh == nullptr)
        {
            fragment_sh = (UShader*)rf->Create(fragment_shader_path, URESOURCE_SHADER);
            fragment_sh->LoadFromFile(shader_path);
            fragment_sh->Create(USHADER_FRAGMENT, defines_array);
        }

        _id = URenderer::GetInstance()->CreateShaderProgram(vertex_sh, fragment_sh);
        InitLocations();

    }
    catch (exception e)
    {
        ULogger::GetInstance()->Message("Error to load shader (xml): " + path, ULOG_MSG_ERROR, ULOG_OUT_MSG);
        return false;
    }
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