#include "UMaterial.h"
#include "..\Renderer\URenderer.h"
#include "..\Basic\ULight.h"
#include "..\UEngine.h"

UMaterial:: UMaterial()
{ 
    skinningTransforms = nullptr;
    skinningTransformsNum = 0;

    spForward = nullptr;
    spDepth = nullptr;
    spNormal = nullptr;
    spDeffered = nullptr;

    ambient.set(0.6f, 0.6f, 0.6f, 1.0f);
    diffuse.set(0.5f, 0.5f, 0.5f, 1.0f);
    specular.set(0.8f, 0.8f, 0.8f, 1.0f);
    emission.set(0.50f, 0.50f, 0.50f, 1.0f);
    shininess = 20.0f;
}

UMaterial:: UMaterial(vec4 amb, vec4 dif, vec4 spec, vec4 emi, float shin, UShaderProgram *_sp) 
{
    ambient = amb;
    diffuse = dif;
    specular = spec;
    emission = emi;
    shininess = shin;
    spForward = _sp;

    spDepth = nullptr;
    spNormal = nullptr;
    spDeffered = nullptr;

    skinningTransforms = nullptr;
    skinningTransformsNum = 0;
}

UMaterial:: UMaterial(vec4 amb, vec4 dif, vec4 spec, vec4 emi, float shin)
{
    ambient = amb;
    diffuse = dif;
    specular = spec;
    emission = emi;
    shininess = shin;

    spForward = nullptr;
    spDepth = nullptr;
    spNormal = nullptr;
    spDeffered = nullptr;

    skinningTransforms = nullptr;
    skinningTransformsNum = 0;
}

bool UMaterial::Load(std::string path)
{
    UXMLFile xml;
    char tex_buffer[UE_MAXCHAR];

    try
    {
        xml.Load(path);
        this->name = xml.GetElement("/xml/material/name/");
        
        emission.x = (float)atof(xml.GetElement("/xml/material/emission/r/").c_str());
        emission.y = (float)atof(xml.GetElement("/xml/material/emission/g/").c_str());
        emission.z = (float)atof(xml.GetElement("/xml/material/emission/b/").c_str());
        emission.w = (float)atof(xml.GetElement("/xml/material/emission/a/").c_str());

        ambient.x = (float)atof(xml.GetElement("/xml/material/ambient/r/").c_str());
        ambient.y = (float)atof(xml.GetElement("/xml/material/ambient/g/").c_str());
        ambient.z = (float)atof(xml.GetElement("/xml/material/ambient/b/").c_str());
        ambient.w = (float)atof(xml.GetElement("/xml/material/ambient/a/").c_str());

        diffuse.x = (float)atof(xml.GetElement("/xml/material/diffuse/r/").c_str());
        diffuse.y = (float)atof(xml.GetElement("/xml/material/diffuse/g/").c_str());
        diffuse.z = (float)atof(xml.GetElement("/xml/material/diffuse/b/").c_str());
        diffuse.w = (float)atof(xml.GetElement("/xml/material/diffuse/a/").c_str());

        specular.x = (float)atof(xml.GetElement("/xml/material/specular/r/").c_str());
        specular.y = (float)atof(xml.GetElement("/xml/material/specular/g/").c_str());
        specular.z = (float)atof(xml.GetElement("/xml/material/specular/b/").c_str());
        specular.w = (float)atof(xml.GetElement("/xml/material/specular/a/").c_str());
            
        shininess = (float)atof(xml.GetElement("/xml/material/shininnes/").c_str());
        //two_sides = atof(xml.GetElement("/xml/material/two_sides/").c_str());
        
        if(xml.isExistElement("/xml/material/shaders/vertex_path/") && xml.isExistElement("/xml/material/shaders/pixel_path/"))
            SetShaderProgram(rf->Load(xml.GetElement("/xml/material/shaders/vertex_path/"), xml.GetElement("/xml/material/shaders/pixel_path/")), URENDER_FORWARD);
        
        if(xml.isExistElement("/xml/material/shaders/normal_vertex_path/") && xml.isExistElement("/xml/material/shaders/normal_pixel_path/"))
            SetShaderProgram(rf->Load(xml.GetElement("/xml/material/shaders/normal_vertex_path/"), xml.GetElement("/xml/material/shaders/normal_pixel_path/")), URENDER_NORMAL);
                
        if(xml.isExistElement("/xml/material/shaders/depth_vertex_path/") && xml.isExistElement("/xml/material/shaders/depth_pixel_path/"))
            SetShaderProgram(rf->Load(xml.GetElement("/xml/material/shaders/depth_vertex_path/"), xml.GetElement("/xml/material/shaders/depth_pixel_path/")), URENDER_DEPTH);
                
        if(xml.isExistElement("/xml/material/shaders/deffered_vertex_path/") && xml.isExistElement("/xml/material/shaders/deffered_pixel_path/"))
            SetShaderProgram(rf->Load(xml.GetElement("/xml/material/shaders/deffered_vertex_path/"), xml.GetElement("/xml/material/shaders/deffered_pixel_path/")), URENDER_DEFFERED);
    
        if(xml.isExistElement("/xml/material/tex_num/"))
        {
            int tex_num = atoi(xml.GetElement("/xml/material/tex_num/").c_str());        
            for(int j = 0; j < tex_num; j++)
            {
                memset(tex_buffer,'\0',255);    
                sprintf_s(tex_buffer,"%d",j);
                std::string current_tex = "/xml/material/textures/tex_" + string(tex_buffer) + "/";
                
                auto tex = pair<UTexture*, unsigned int>( 
                            dynamic_cast<UTexture*>(rf->Load(xml.GetElement(current_tex + "path/"), URESOURCE_TEXTURE)), 
                            atoi(xml.GetElement(current_tex + "channel/").c_str()));
                
                tex.first->name = xml.GetElement(current_tex + "name/");
                AddTexture(tex);
            }
        }
        
        if(xml.isExistElement("/xml/material/param_num/"))
        {
            int param_num = atoi(xml.GetElement("/xml/material/param_num/").c_str());        
            for(int j = 0; j < param_num; j++)
            {
                memset(tex_buffer,'\0',255);    
                sprintf_s(tex_buffer,"%d",j);
                std::string current_param = "/xml/material/params/param_" + string(tex_buffer) + "/";
                
                params[xml.GetElement(current_param + "name/")] = xml.GetElementf(current_param + "value/");                
            }
        }
    }
    catch(exception e)
    {
        ULogger::GetInstance()->Message("Error to load model (xml): " + path, ULOG_MSG_ERROR, ULOG_OUT_MSG);
        return false;
    }
    return true;
}

UShaderProgram* UMaterial::GetShaderProgram(URENDER_TYPE type)
{
    if(type == URENDER_FORWARD) 
        return spForward;
    else if(type == URENDER_DEPTH)
    {
        return spDepth;
    }
    else if(type == URENDER_NORMAL)
    {
        return spNormal;
    }
    else if(type == URENDER_DEFFERED)
    {
        return spDeffered;
    }

    return nullptr;
}

void UMaterial::SetShaderProgram(UShaderProgram *_sp, URENDER_TYPE type)
{
    if(type == URENDER_FORWARD) 
        spForward = _sp; 
    else if(type == URENDER_DEPTH)
        spDepth = _sp;
    else if(type == URENDER_NORMAL)
        spNormal = _sp;
    else if(type == URENDER_DEFFERED)
        spDeffered = _sp;
}

void UMaterial::Render(URENDER_TYPE type)
{
    UShaderProgram *sp = GetShaderProgram(type);
    
    if(sp == nullptr)
        return;
    
    URenderer *render = URenderer::GetInstance();

    render->SetShaderProgram(sp);
    render->SetupCameraForShaderProgram(GetShaderProgram(type), render->modelView);
    OPENGL_CHECK_FOR_ERRORS();

    UShaderProgram::UUniformLocations locs = sp->locations;

    if(skinningTransformsNum > 0 && skinningTransforms != nullptr)
    {
        render->Uniform1(locs.skinning_transformsNum, skinningTransformsNum);
        render->UniformMatrix4(locs.skinning_transforms, skinningTransformsNum, reinterpret_cast<float*>(skinningTransforms[0].m));
    }

    if(type == URENDER_FORWARD || type == URENDER_DEFFERED)
    {
        render->Uniform4(locs.material_ambient,  1, ambient.v);
        render->Uniform4(locs.material_diffuse,  1, diffuse.v);
        render->Uniform4(locs.material_specular,  1, specular.v);
        render->Uniform4(locs.material_emission,  1, emission.v);
        render->Uniform1(locs.material_shininess,  1, &shininess);
                
        auto sceneLights = render->GetCurrentScene()->GetLights();
        auto lights = render->GetCurrentScene()->lightParams;

        int cur = 0;
        for(unsigned int i = 0; i < lights.count; i++)
        {
            sceneLights[lights.lightIndex[i]]->SetShadowTexture(locs.light_depthTextures, i);
            cur++;
            
            if(cur > MAX_LIGHTS)
                break;
        }
        
        OPENGL_CHECK_FOR_ERRORS();

        if(lights.count != 0)
        {
            render->Uniform1(locs.lightsNum, (int)lights.count);
            render->Uniform4(locs.light_position, lights.count, reinterpret_cast<float*>(lights.position));
            render->Uniform4(locs.light_ambient, lights.count, reinterpret_cast<float*>(lights.ambient));
            render->Uniform4(locs.light_diffuse, lights.count, reinterpret_cast<float*>(lights.diffuse));
            render->Uniform4(locs.light_specular, lights.count, reinterpret_cast<float*>(lights.specular));
            render->Uniform3(locs.light_attenuation, lights.count, reinterpret_cast<float*>(lights.attenuation));
            render->Uniform3(locs.light_spotDirection, lights.count, reinterpret_cast<float*>(lights.spotDirection));
            render->Uniform1(locs.light_spotExponent, lights.count, reinterpret_cast<float*>(lights.spotExponent));
            render->Uniform1(locs.light_spotCosCutoff, lights.count, reinterpret_cast<float*>(lights.spotCosCutoff));
            render->UniformMatrix4(locs.light_transform, lights.count, reinterpret_cast<float*>(lights.transforms));
        }
    }

    OPENGL_CHECK_FOR_ERRORS();
    params["time"] = (float)GetTickCount();

    if(params.size() > 0)
    {
        map<string, float>::iterator i = params.begin();
        do
        {
            render->CacheUniform1((*i).first, 1, &(*i).second);
            OPENGL_CHECK_FOR_ERRORS();

            i++;
        }while(i != params.end());        
    }

    OPENGL_CHECK_FOR_ERRORS();

    if(type == URENDER_FORWARD || type == URENDER_DEFFERED || type == URENDER_NORMAL)
    {
        for each(auto el in textures)
        {            
            render->BindTexture(el.first, el.second);
            render->CacheUniform1(el.first->name, el.second);
            OPENGL_CHECK_FOR_ERRORS();
        }
    }

}

void UMaterial::Render(UShaderProgram* sp)
{
    //not implemented yet
}