#include "ULight.h"
#include "..\Resources\UResourceFactory.h"
#include "..\Resources\UModel.h"
#include "..\Utils\UConfig.h"
#include "..\UEngine.h"
#include "..\GameMechanics\UGameMechanics.h"

ULight::ULight()
{
    ULight(&UEngine::rf, vec4_zero);

}

ULight::ULight(UResourceFactory* rf,  vec4 pos, bool cast)
{
    this->castShadows = cast;
    ULight(&UEngine::rf, pos);
}

ULight::ULight(UResourceFactory* rf, vec4 pos)
{
    castShadows = false;
    
    ambient.set(0.5f, 0.5f, 0.5f, 1.0f);
    diffuse.set(0.70f, 0.70f, 0.70f, 1.0f);
    specular.set(0.9f, 0.9f, 0.9f, 1.0f);
    attenuation.set(0.5f, 0.1f, 0.001f);
    spotDirection.set(-1.0f, -2.0f, -1.0f);
    spotCosCutoff = cosf(45.0f * math_radians);
    spotExponent = 20.0f;

    InitModel(rf);
    
    int size = atoi(UConfig::GetInstance()->GetParam("/xml/config/depth_texture_size/").c_str());
    /*for(int i = 0; i < 6; i++)
    {
        auto tex = dynamic_cast<UTexture*>(rf->Create(URESOURCE_TEXTURE));
        tex->Create(size, size, UTEXTURE_DEPTH);
        
        depthTextures.push_back(tex);

        UCamera cam;

        //cam.LookAt(position, -position, vec3_y);
        cam.Ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.0f, 10000.0f);
        
        //cam.Perspective(90.0f, URenderer::GetInstance()->GetWidth()/URe nderer::GetInstance()->GetHeight(), 0.001f, 10000.0f);    
        cameras.push_back(cam);
    }*/

    auto tex = dynamic_cast<UTexture*>(rf->Create(URESOURCE_TEXTURE));
    tex->Create(size, size, UTEXTURE_DEPTH_SHADOW);        
    depthTextures.push_back(tex);
    
    UCamera cam;
    //size = 10.0f;
    cam.Perspective(spotCosCutoff * math_degrees *2.0f, (float)URenderer::GetInstance()->GetWidth()/(float)URenderer::GetInstance()->GetHeight(), 0.5f, 100000.0f);
    //cam.Ortho(-size, size, -size, size, -10.0f, 10000.0f);
    cameras.push_back(cam);

    local.position = pos;
}

 mat4 ULight:: GetLightTransform()
 {
    static const mat4 bias(
            0.5f, 0.0f, 0.0f, 0.5f,
            0.0f, 0.5f, 0.0f, 0.5f,
            0.0f, 0.0f, 0.5f, 0.5f,
            0.0f, 0.0f, 0.0f, 1.0f
    );

    mat4 view           = GLFromEuler(cameras[0].GetRotation()) * GLTranslation(-cameras[0].GetPosition());
    mat4 viewProjection = bias * cameras[0].GetProjection() * view;
    return viewProjection.m;
    //URenderer::GetInstance()->CacheUniformMatrix(light+ "transform", 1, viewProjection.m);    
}
 
void ULight:: SetShadowTexture(unsigned int location, int i)
{    
    if(castShadows)
    {
        URenderer::GetInstance()->BindTexture(depthTextures[0], SHADOW_TEXTURE_CHANNEL + i);
        URenderer::GetInstance()->Uniform1(max(location + i, location), SHADOW_TEXTURE_CHANNEL + i);
    }
}

void ULight:: SetLightTransform(string light)
{
    // Deprecated!
    static const mat4 bias(
            0.5f, 0.0f, 0.0f, 0.5f,
            0.0f, 0.5f, 0.0f, 0.5f,
            0.0f, 0.0f, 0.5f, 0.5f,
            0.0f, 0.0f, 0.0f, 1.0f
    );

    mat4 view           = GLFromEuler(cameras[0].GetRotation()) * GLTranslation(-cameras[0].GetPosition());
    mat4 viewProjection = bias * cameras[0].GetProjection() * view;
    
    URenderer::GetInstance()->CacheUniformMatrix4(light+ "transform", 1, viewProjection.m);    

}

void ULight:: SetShaderParameters(int i)
{    
    // Deprecated!
    char buffer[MAXCHAR];
    memset(buffer, '\0', MAXCHAR);
    sprintf_s(buffer, "light[%d].", i);

    string light = buffer;
    
    auto renderer = URenderer::GetInstance();

    renderer->CacheUniform4( light + "position",1, local.position.v);
    renderer->CacheUniform4( light + "ambient",1, ambient.v);
    renderer->CacheUniform4( light + "diffuse",1, diffuse.v);
    renderer->CacheUniform4( light + "specular",1, specular.v);
    renderer->CacheUniform3( light + "attenuation",1, attenuation.v);
    renderer->CacheUniform3( light + "spotDirection",1, spotDirection.v);
    renderer->CacheUniform1( light + "spotExponent",1, &spotExponent);
    renderer->CacheUniform1( light + "spotCosCutoff",1, &spotCosCutoff);
    
    SetLightTransform(light);
}

std::vector<UTexture*> ULight::GetDepthTextures()
{
    return depthTextures; 
}

std::vector<UCamera> ULight::GetCameras()
{ 
    return cameras; 
}

ULight::~ULight(void)
{
}

void ULight::InitModel(UResourceFactory* rf)
{
    model = dynamic_cast<UModel*>(rf->Create(URESOURCE_MODEL));
    model->Load("data\\Models\\light_model.xml");

    components.push_back(model);
}

void ULight:: Update(double delta)
{    
    UGameObject::Update(delta);

    //spotDirection = -vec3_y;
    spotDirection = -normalize(world*(local.position));
//    world
    cameras[0].LookAt(world*(local.position) , spotDirection + world * local.position, world * vec3_y);
}