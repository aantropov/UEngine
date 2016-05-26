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

ULight::ULight(UResourceFactory* rf, vec4 pos, bool cast)
{
    this->castShadows = cast;
    ULight(&UEngine::rf, pos);
}

ULight::ULight(UResourceFactory* rf, vec4 pos)
{
    type = ULIGHT_TYPE::ULIGHT_TYPE_DIRECTIONAL;
    castShadows = false;

    shadowDistanceMin = 0.1f;
    shadowDistanceMax = 150.0f;

    ambient.set(0.5f, 0.5f, 0.5f, 1.0f);
    diffuse.set(0.70f, 0.70f, 0.70f, 1.0f);
    specular.set(0.9f, 0.9f, 0.9f, 1.0f);
    attenuation.set(0.5f, 0.1f, 0.001f);
    lightDirection.set(-1.0f, -2.0f, -1.0f);
    spotCosHalfAngle = -1.0f;
    spotAngle = 60.0f;
    spotExponent = 5.0f;

    SetSpotCosCutoff(spotAngle);

    InitModel(rf);

    UCamera cam;
    cam.SetPosition(pos);
    cameras.push_back(cam);
    UpdateCamera();

    local.position = pos;
}

mat4 ULight::GetLightTransform()
{
    static const mat4 bias(
        0.5f, 0.0f, 0.0f, 0.5f,
        0.0f, 0.5f, 0.0f, 0.5f,
        0.0f, 0.0f, 0.5f, 0.5f,
        0.0f, 0.0f, 0.0f, 1.0f
        );

    mat4 view = GLFromEuler(cameras[0].GetRotation()) * GLTranslation(-cameras[0].GetPosition());
    mat4 viewProjection = bias * cameras[0].GetProjection() * view;
    return viewProjection.m;
}

void ULight::SetShadowTexture(unsigned int location, int i)
{
    if (castShadows)
    {
        URenderer::GetInstance()->BindTexture(depthTextures[0], SHADOW_TEXTURE_CHANNEL + i);
        URenderer::GetInstance()->Uniform1(max(location + i, location), SHADOW_TEXTURE_CHANNEL + i);
    }
}

void ULight::SetLightTransform(string light)
{
    // Deprecated!
    static const mat4 bias(
        0.5f, 0.0f, 0.0f, 0.5f,
        0.0f, 0.5f, 0.0f, 0.5f,
        0.0f, 0.0f, 0.5f, 0.5f,
        0.0f, 0.0f, 0.0f, 1.0f
        );

    mat4 view = GLFromEuler(cameras[0].GetRotation()) * GLTranslation(-cameras[0].GetPosition());
    mat4 viewProjection = bias * cameras[0].GetProjection() * view;

    URenderer::GetInstance()->CacheUniformMatrix4(light + "transform", 1, viewProjection.m);
}

void ULight::SetShaderParameters(int i)
{
    // Deprecated!
    char buffer[MAXCHAR];
    memset(buffer, '\0', MAXCHAR);
    sprintf_s(buffer, "light[%d].", i);

    string light = buffer;

    auto renderer = URenderer::GetInstance();

    vec4 position = local.position;
    position.w = shadowDistanceMax + shadowDistanceMin;

    renderer->CacheUniform4(light + "position", 1, position.v);
    renderer->CacheUniform4(light + "ambient", 1, ambient.v);
    renderer->CacheUniform4(light + "diffuse", 1, diffuse.v);
    renderer->CacheUniform4(light + "specular", 1, specular.v);
    renderer->CacheUniform3(light + "attenuation", 1, attenuation.v);
    renderer->CacheUniform3(light + "spotDirection", 1, lightDirection.v);
    renderer->CacheUniform1(light + "spotExponent", 1, &spotExponent);
    renderer->CacheUniform1(light + "spotCosCutoff", 1, &spotCosHalfAngle);

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
    model->LoadFromFile("data\\Models\\light_model.xml");

    //components.push_back(model);
}

void ULight::Update(double delta)
{
    UGameObject::Update(delta);
    UpdateCamera();
}

void ULight::UpdateCamera()
{
    lightDirection = -normalize(world*(local.position));

    if (type == ULIGHT_TYPE::ULIGHT_TYPE_SPOT)
        cameras[0].Perspective(spotAngle, 1.0f, shadowDistanceMin, shadowDistanceMax);
    else if (type == ULIGHT_TYPE::ULIGHT_TYPE_DIRECTIONAL)
    {
        auto size = 0.5f * (shadowDistanceMax - shadowDistanceMin);
        //size = 1000.0f;
        cameras[0].Ortho(-size, size, -size, size, shadowDistanceMin, shadowDistanceMax);
    }

    cameras[0].LookAt(world*(local.position), lightDirection + world * local.position, world * vec3_y);
}

void ULight::UpdateDepthTextures()
{
    if (castShadows && depthTextures.size() == 0)
    {
        int size = atoi(UConfig::GetInstance()->GetParam("/xml/config/depth_texture_size/").c_str());
        
        auto tex = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
        //tex->Create(size, size, UTEXTURE_DEPTH_SHADOW, UTEXTURE_FILTER_LINEAR, UTEXTURE_WRAP_CLAMP_TO_EDGE);
        tex->SetMipMap(false);
        tex->Create(size, size, UTEXTURE_RG32F, UTEXTURE_FILTER::UTEXTURE_FILTER_LINEAR, UTEXTURE_WRAP::UTEXTURE_WRAP_CLAMP_TO_EDGE);
        depthTextures.push_back(tex);
    }
}