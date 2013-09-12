#include "UResourceFactory.h"
#include "..\Renderer\UShader.h"
#include "UTexture.h"
#include "UModel.h"
#include "UPostEffect.h"
#include "..\Renderer\UShaderProgram.h"
#include "..\Lua\UScript.h"

UResource* UResourceFactory:: Create(URESOURCE_TYPE type)
{
    char buffer[UE_MAXCHAR];
    memset(buffer, '\0', UE_MAXCHAR);
    sprintf_s(buffer, "\\memory\\%d%d\\", (int)type, unique_id++);
    std::string path = buffer;

    ULogger::GetInstance()->Message("Creating resource: \"" + path + "\"");

    if(type == URESOURCE_SHADER)
    {   
        UShader* temp = new UShader();
        temp->rf = this;
        temp->resourceId = path;
        resources.push_back(new UElement(temp, path));        
        return temp;
    }
    else if(type == URESOURCE_TEXTURE)
    {
        UTexture* temp = new UTexture();
        temp->rf = this;
        temp->resourceId = path;
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if(type == URESOURCE_MODEL)
    {
        UModel* temp = new UModel();
        temp->rf = this;
        temp->resourceId = path;
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if(type == URESOURCE_POST_EFFECT)
    {
        UPostEffect* temp = new UPostEffect();
        temp->rf = this;
        temp->resourceId = path;
        temp->Load(path);                
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if(type == URESOURCE_SHADER_PROGRAM)
    {
        UShaderProgram* temp = new UShaderProgram();
        temp->rf = this;
        temp->resourceId = path;
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if(type == URESOURCE_SKIN_ANIMATION)
    {
        UAnimation* temp = new UAnimation();
        temp->rf = this;
        temp->resourceId = path;
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if(type == URESOURCE_MATERIAL)
    {
        UMaterial* temp = new UMaterial();
        temp->rf = this;
        temp->resourceId = path;
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if(type == URESOURCE_SCRIPT)
    {
        UScript* temp = new UScript();
        temp->rf = this;
        temp->resourceId = path;
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else 
        return nullptr;
}

UResource* UResourceFactory:: Get(std::string path)
{
    for(unsigned int i = 0 ; i < resources.size() ; i++ )
    {
        if(path == resources[i]->GetId()){
            return resources[i]->GetResource();                        
        }
    }
    return nullptr;
}

bool UResourceFactory:: Add(std::string path, UResource* resource)
{
    UResource* res = Get(path);
    if(res != nullptr)
        return false;

    resources.push_back(new UElement(resource, path));
    return true;
}

UShaderProgram* UResourceFactory:: Load(std::string vp, std::string pp)
{
    string path = "\\shader_program\\" + vp + "\\" + pp;

    UShaderProgram* res = dynamic_cast<UShaderProgram*>(Get(path));
    if(res != nullptr)
        return res;

    ULogger::GetInstance()->Message("Loading resource: \"" + path + "\"");

    UShaderProgram* temp = new UShaderProgram();
    temp->rf = this;
    temp->Load(vp, pp);                    
    resources.push_back(new UElement(temp, path));
    return temp;

}

UResource* UResourceFactory:: Load(std::string path, URESOURCE_TYPE type)
{
    UResource* res = Get(path);
    if(res != nullptr)
        return res;

    ULogger::GetInstance()->Message("Loading resource: \"" + path + "\"");
    if(type == URESOURCE_SHADER)
    {        
        UShader* temp = new UShader();
        temp->rf = this;
        temp->Load(path);                    
        resources.push_back(new UElement(temp, path));        
        return temp;    
    }
    else if(type == URESOURCE_TEXTURE)
    {
        UTexture* temp = new UTexture();
        temp->rf = this;
        temp->Load(path);                    
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if(type == URESOURCE_MODEL)
    {
        UModel* temp = new UModel();
        temp->rf = this;
        temp->Load(path);                    
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if(type == URESOURCE_POST_EFFECT)
    {
        UPostEffect* temp = new UPostEffect();
        temp->rf = this;
        temp->Load(path);                    
        resources.push_back(new UElement(temp, path));
        return temp;
    }    
    else if(type == URESOURCE_SKIN_ANIMATION)
    {
        UAnimation* temp = new UAnimation();
        temp->rf = this;
        temp->Load(path);                    
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if(type == URESOURCE_MATERIAL)
    {
        UMaterial* temp = new UMaterial();
        temp->rf = this;
        temp->Load(path);                    
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else if(type == URESOURCE_SCRIPT)
    {
        UScript* temp = new UScript();
        temp->rf = this;
        temp->Load(path);                    
        resources.push_back(new UElement(temp, path));
        return temp;
    }
    else
        return nullptr;
}

void UResourceFactory::Release(std::string path)
{
    for(unsigned int i = 0 ; i < resources.size() ; i++ )
    {
        if(path == resources[i]->GetId())
        {
            delete resources[i]->GetResource();
            resources[i]->SetResource(NULL);    
            delete resources[i];
            resources.erase(resources.begin()+i);
        }
    }
}    

void UResourceFactory::Release(UResource *resource)
{
    for(unsigned  int i = 0 ; i < resources.size() ; i++ )
    {
        if(resource == resources[i]->GetResource())
        {
            delete resources[i]->GetResource();
            resources[i]->SetResource(NULL);
            delete resources[i];            
            resources.erase(resources.begin()+i);
        }
    }
}    

void UResourceFactory::ReleaseAll()
{
    for(unsigned int i = 0 ; i < resources.size() ; i++ )
    {    
        delete resources[i]->GetResource();
        resources[i]->SetResource(NULL);
        delete resources[i];                
    }
    resources.clear();
}