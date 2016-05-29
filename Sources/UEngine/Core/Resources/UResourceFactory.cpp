#include "UResourceFactory.h"
#include "..\Renderer\UShader.h"
#include "UTexture.h"
#include "UModel.h"
#include "UPostEffect.h"
#include "UCubemap.h"
#include "..\Renderer\UShaderProgram.h"
#include "..\Lua\UScript.h"

UResource* UResourceFactory::Create(UResourceType type)
{
    char buffer[UE_MAXCHAR];
    memset(buffer, '\0', UE_MAXCHAR);
    sprintf_s(buffer, "\\memory\\%d%d\\", (int)type, unique_id++);
    std::string path = buffer;
    return Create(path, type);
}

UResource* UResourceFactory::Create(std::string path, UResourceType type)
{
    ULogger::GetInstance()->Message("Creating resource: \"" + path + "\"");

    if (type == UResourceType::Shader)
    {
        UShader* temp = new UShader();
        temp->rf = this;
        temp->resourceId = path;
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if (type == UResourceType::Texture)
    {
        UTexture* temp = new UTexture();
        temp->rf = this;
        temp->resourceId = path;
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if (type == UResourceType::Cubemap)
    {
        UCubemap* temp = new UCubemap();
        temp->rf = this;
        temp->resourceId = path;
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if (type == UResourceType::Model)
    {
        UModel* temp = new UModel();
        temp->rf = this;
        temp->resourceId = path;
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if (type == UResourceType::PostEffect)
    {
        UPostEffect* temp = new UPostEffect();
        temp->rf = this;
        temp->resourceId = path;
        temp->LoadFromFile(path);
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if (type == UResourceType::ShaderProgram)
    {
        UShaderProgram* temp = new UShaderProgram();
        temp->rf = this;
        temp->resourceId = path;
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if (type == UResourceType::Animation)
    {
        UAnimation* temp = new UAnimation();
        temp->rf = this;
        temp->resourceId = path;
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if (type == UResourceType::Material)
    {
        UMaterial* temp = new UMaterial();
        temp->rf = this;
        temp->resourceId = path;
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if (type == UResourceType::Script)
    {
        UScript* temp = new UScript();
        temp->rf = this;
        temp->resourceId = path;
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if (type == UResourceType::Cubemap)
    {
        UCubemap* temp = new UCubemap();
        temp->rf = this;
        temp->resourceId = path;
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else
        return nullptr;
}

UResource* UResourceFactory::Get(std::string path)
{
    for (unsigned int i = 0; i < resources.size(); i++)
    {
        if (path == resources[i]->GetId()) {
            return resources[i]->GetResource();
        }
    }
    return nullptr;
}

bool UResourceFactory::Add(std::string path, UResource* resource)
{
    UResource* res = Get(path);
    if (res != nullptr)
        return false;

    resources.push_back(new UElement(resource, path));
    return true;
}

UResource* UResourceFactory::Load(std::string path, UResourceType type)
{
    UResource* res = Get(path);
    if (res != nullptr)
        return res;

    ULogger::GetInstance()->Message("Loading resource: \"" + path + "\"");
    if (type == UResourceType::Shader)
    {
        UResource* temp = new UShader();
        temp->rf = this;
        temp->LoadFromFile(path);
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if (type == UResourceType::ShaderProgram)
    {
        UResource* temp = new UShaderProgram();
        temp->rf = this;
        temp->LoadFromFile(path);
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if (type == UResourceType::Texture)
    {
        UResource* temp = new UTexture();
        temp->rf = this;
        temp->LoadFromFile(path);
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if (type == UResourceType::Cubemap)
    {
        UResource* temp = new UCubemap();
        temp->rf = this;
        temp->LoadFromFile(path);
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if (type == UResourceType::Model)
    {
        UResource* temp = new UModel();
        temp->rf = this;
        temp->LoadFromFile(path);
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if (type == UResourceType::PostEffect)
    {
        UResource* temp = new UPostEffect();
        temp->rf = this;
        temp->LoadFromFile(path);
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if (type == UResourceType::Animation)
    {
        UAnimation* temp = new UAnimation();
        temp->rf = this;
        temp->LoadFromFile(path);
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if (type == UResourceType::Material)
    {
        UResource* temp = new UMaterial();
        temp->rf = this;
        temp->LoadFromFile(path);
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if (type == UResourceType::Script)
    {
        UResource* temp = new UScript();
        temp->rf = this;
        temp->LoadFromFile(path);
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else
        return nullptr;
}

void UResourceFactory::Release(std::string path)
{
    for (unsigned int i = 0; i < resources.size(); i++)
    {
        if (path == resources[i]->GetId())
        {
            delete resources[i]->GetResource();
            resources[i]->SetResource(NULL);
            delete resources[i];
            resources.erase(resources.begin() + i);
        }
    }
}

void UResourceFactory::Release(UResource *resource)
{
    for (unsigned int i = 0; i < resources.size(); i++)
    {
        if (resource == resources[i]->GetResource())
        {
            delete resources[i]->GetResource();
            resources[i]->SetResource(NULL);
            delete resources[i];
            resources.erase(resources.begin() + i);
        }
    }
}

void UResourceFactory::ReleaseAll()
{
    for (unsigned int i = 0; i < resources.size(); i++)
    {
        delete resources[i]->GetResource();
        resources[i]->SetResource(NULL);
        delete resources[i];
    }
    resources.clear();
}