#include "UMaterial.h"
#include "..\Renderer\URenderer.h"
#include "..\Basic\ULight.h"
#include "..\UEngine.h"
#include "UCubemap.h"

UMaterial::UMaterial()
{
    skinningTransforms = nullptr;
    skinningTransformsNum = 0;

    shader_forward = nullptr;
    shader_depth = nullptr;
    shader_normal = nullptr;
    shader_deffered = nullptr;

    diffuse.set(0.5f, 0.5f, 0.5f, 1.0f);
    specular.set(0.8f, 0.8f, 0.8f, 1.0f);
    emission.set(0.50f, 0.50f, 0.50f, 1.0f);
    shininess = 20.0f;
}

UMaterial::UMaterial(vec4 dif, vec4 spec, vec4 emi, float shin, UShaderProgram *_sp)
{
    diffuse = dif;
    specular = spec;
    emission = emi;
    shininess = shin;
    shader_forward = _sp;

    shader_depth = nullptr;
    shader_normal = nullptr;
    shader_deffered = nullptr;

    skinningTransforms = nullptr;
    skinningTransformsNum = 0;
}

UMaterial::UMaterial(vec4 dif, vec4 spec, vec4 emi, float shin)
{
    diffuse = dif;
    specular = spec;
    emission = emi;
    shininess = shin;

    shader_forward = nullptr;
    shader_depth = nullptr;
    shader_normal = nullptr;
    shader_deffered = nullptr;

    skinningTransforms = nullptr;
    skinningTransformsNum = 0;
}

bool UMaterial::Load(UXMLFile& xml, std::string path)
{
    char tex_buffer[UE_MAXCHAR];
    try
    {
        this->name = xml.GetElement(path + "material/name/");

        emission.x = (float)atof(xml.GetElement(path + "material/emission/r/").c_str());
        emission.y = (float)atof(xml.GetElement(path + "material/emission/g/").c_str());
        emission.z = (float)atof(xml.GetElement(path + "material/emission/b/").c_str());
        emission.w = (float)atof(xml.GetElement(path + "material/emission/a/").c_str());

        /*
        ambient.x = (float)atof(xml.GetElement(path + "material/ambient/r/").c_str());
        ambient.y = (float)atof(xml.GetElement(path + "material/ambient/g/").c_str());
        ambient.z = (float)atof(xml.GetElement(path + "material/ambient/b/").c_str());
        ambient.w = (float)atof(xml.GetElement(path + "material/ambient/a/").c_str());
        */

        diffuse.x = (float)atof(xml.GetElement(path + "material/diffuse/r/").c_str());
        diffuse.y = (float)atof(xml.GetElement(path + "material/diffuse/g/").c_str());
        diffuse.z = (float)atof(xml.GetElement(path + "material/diffuse/b/").c_str());
        diffuse.w = (float)atof(xml.GetElement(path + "material/diffuse/a/").c_str());

        specular.x = (float)atof(xml.GetElement(path + "material/specular/r/").c_str());
        specular.y = (float)atof(xml.GetElement(path + "material/specular/g/").c_str());
        specular.z = (float)atof(xml.GetElement(path + "material/specular/b/").c_str());
        specular.w = (float)atof(xml.GetElement(path + "material/specular/a/").c_str());

        shininess = (float)atof(xml.GetElement(path + "material/shininess/").c_str());
        //two_sides = atof(xml.GetElement(path + "material/two_sides/").c_str());

        if (xml.isExistElement(path + "material/shaders/forward/"))
        {
            UShaderProgram* shader_program = (UShaderProgram*)rf->Create(xml.GetPath() + " forward shader program", UResourceType::ShaderProgram);
            shader_program->Load(xml, path + "material/shaders/forward/");

            SetShaderProgram(shader_program, URenderPass::Forward);
        }

        if (xml.isExistElement(path + "material/shaders/normal/"))
        {
            UShaderProgram* shader_program = (UShaderProgram*)rf->Create(xml.GetPath() + " normal shader program", UResourceType::ShaderProgram);
            shader_program->Load(xml, path + "material/shaders/normal/");

            SetShaderProgram(shader_program, URenderPass::Normal);
        }

        if (xml.isExistElement(path + "material/shaders/depth/"))
        {
            UShaderProgram* shader_program = (UShaderProgram*)rf->Create(xml.GetPath() + " depth shader program", UResourceType::ShaderProgram);
            shader_program->Load(xml, path + "material/shaders/depth/");

            SetShaderProgram(shader_program, URenderPass::Depth);
        }

        if (xml.isExistElement(path + "material/shaders/deffered/"))
        {
            UShaderProgram* shader_program = (UShaderProgram*)rf->Create(xml.GetPath() + " deffered shader program", UResourceType::ShaderProgram);
            shader_program->Load(xml, path + "material/shaders/deffered/");

            SetShaderProgram(shader_program, URenderPass::Deffered);
        }

        if (xml.isExistElement(path + "material/tex_num/"))
        {
            int tex_num = atoi(xml.GetElement(path + "material/tex_num/").c_str());
            for (int j = 0; j < tex_num; j++)
            {
                memset(tex_buffer, '\0', 255);
                sprintf_s(tex_buffer, "%d", j);
                std::string current_tex = path + "material/textures/tex_" + string(tex_buffer) + "/";

                pair<UTexture*, unsigned int> tex;
                if (!xml.isExistElement(current_tex + "name/"))
                {
                    tex = pair<UTexture*, unsigned int>(
                        dynamic_cast<UTexture*>(rf->Load(xml.GetElement(current_tex + "path/"), UResourceType::Texture)),
                        atoi(xml.GetElement(current_tex + "channel/").c_str()));
                }
                else
                {
                    tex = pair<UTexture*, unsigned int>(
                        dynamic_cast<UTexture*>((UTexture*)rf->Create(UResourceType::Texture)),
                        atoi(xml.GetElement(current_tex + "channel/").c_str()));

                    tex.first->LoadFromFile(xml.GetElement(current_tex + "path/"));
                    tex.first->name = xml.GetElement(current_tex + "name/");
                }

                AddUniformUnit(tex.second, tex.first);
            }
        }


        if (xml.isExistElement(path + "material/cubemaps_num/"))
        {
            int tex_num = atoi(xml.GetElement(path + "material/cubemaps_num/").c_str());
            for (int j = 0; j < tex_num; j++)
            {
                memset(tex_buffer, '\0', 255);
                sprintf_s(tex_buffer, "%d", j);
                std::string current_tex = path + "material/cubemaps/cubemap_" + string(tex_buffer) + "/";

                auto tex = pair<UCubemap*, unsigned int>(
                    dynamic_cast<UCubemap*>(rf->Load(xml.GetElement(current_tex + "path/"), UResourceType::Cubemap)),
                    atoi(xml.GetElement(current_tex + "channel/").c_str()));

                tex.first->name = xml.GetElement(current_tex + "name/");
                AddUniformUnit(tex.second, tex.first);
            }
        }

        xml.TryGetElementb(path + "material/shadow_caster/", is_shadow_caster);
        xml.TryGetElementb(path + "material/depth_write/", is_depth_write_enabled);
        xml.TryGetElementi(path + "material/queue/", queue);
        xml.TryGetElementEnum<UBlendMode, UBlendModeHelper>(path + "material/blend_mode/", blend_mode);
        xml.TryGetElementEnum<UDepthFunc, UDepthFuncHelper>(path + "material/depth_func/", depth_func);

        if (xml.isExistElement(path + "material/param_num/"))
        {
            int param_num = atoi(xml.GetElement(path + "material/param_num/").c_str());
            for (int j = 0; j < param_num; j++)
            {
                memset(tex_buffer, '\0', 255);
                sprintf_s(tex_buffer, "%d", j);
                std::string current_param = path + "material/params/param_" + string(tex_buffer) + "/";

                params[xml.GetElement(current_param + "name/")] = UUniformParam(xml.GetElementf(current_param + "value/"));
            }
        }
    }
    catch (exception e)
    {
        ULogger::GetInstance()->Message("Error to load material (xml): " + path, ULogType::Error, ULogTarget::MsgBox);
        return false;
    }
    return true;
}

UShaderProgram* UMaterial::GetShaderProgram(URenderPass type)
{
    if (type == URenderPass::Forward)
        return shader_forward;
    else if (type == URenderPass::Depth || type == URenderPass::DepthShadow)
        return shader_depth;
    else if (type == URenderPass::Normal)
        return shader_normal;
    else if (type == URenderPass::Deffered)
        return shader_deffered;
    return nullptr;
}

void UMaterial::SetShaderProgram(UShaderProgram *_sp, URenderPass type)
{
    if (type == URenderPass::Forward)
        shader_forward = _sp;
    else if (type == URenderPass::Depth)
        shader_depth = _sp;
    else if (type == URenderPass::Normal)
        shader_normal = _sp;
    else if (type == URenderPass::Deffered)
        shader_deffered = _sp;
}

void UMaterial::Render(URenderPass type, int light_index)
{
    UShaderProgram *sp = GetShaderProgram(type);

    if (sp == nullptr)
        return;

    URenderer *render = URenderer::GetInstance();

    render->SetShaderProgram(sp);
    render->SetupCameraForShaderProgram(GetShaderProgram(type), render->model_view);
    OPENGL_CHECK_FOR_ERRORS();

    UShaderProgram::UUniformLocations locs = sp->locations;

    if (skinningTransformsNum > 0 && skinningTransforms != nullptr)
    {
        render->Uniform1(locs.skinning_transformsNum, skinningTransformsNum);
        render->UniformMatrix4(locs.skinning_transforms, skinningTransformsNum, reinterpret_cast<float*>(skinningTransforms[0].m));
    }

    if (type == URenderPass::Forward || type == URenderPass::Deffered)
    {
        render->Uniform4(locs.material_diffuse, 1, diffuse.v);
        render->Uniform4(locs.material_specular, 1, specular.v);
        render->Uniform4(locs.material_emission, 1, emission.v);
        render->Uniform1(locs.material_shininess, 1, &shininess);

        auto sceneLights = render->GetCurrentScene()->GetLights();
        auto lights = render->GetCurrentScene()->light_params;

        if (lights.count != 0)
        {
            if (type == URenderPass::Deffered)
            {
                render->Uniform4(locs.light_position, 1, reinterpret_cast<float*>(&lights.position[light_index]));
                render->Uniform4(locs.light_ambient, 1, reinterpret_cast<float*>(&lights.ambient[light_index]));
                render->Uniform4(locs.light_diffuse, 1, reinterpret_cast<float*>(&lights.diffuse[light_index]));
                render->Uniform4(locs.light_specular, 1, reinterpret_cast<float*>(&lights.specular[light_index]));
                render->Uniform3(locs.light_attenuation, 1, reinterpret_cast<float*>(&lights.attenuation[light_index]));
                render->Uniform3(locs.light_spotDirection, 1, reinterpret_cast<float*>(&lights.spotDirection[light_index]));
                render->Uniform1(locs.light_spotExponent, 1, reinterpret_cast<float*>(&lights.spotExponent[light_index]));
                render->Uniform1(locs.light_spotCosCutoff, 1, reinterpret_cast<float*>(&lights.spotCosCutoff[light_index]));
                render->Uniform1(locs.light_type, 1, reinterpret_cast<int*>(&lights.types[light_index]));
                render->UniformMatrix4(locs.light_transform, 1, reinterpret_cast<float*>(&lights.transforms[light_index]));

                if (sceneLights[lights.light_index[light_index]]->IsShadowCaster())
                    sceneLights[lights.light_index[light_index]]->SetShadowTexture(locs.light_depthTextures, 0);
            }
            else
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
                render->Uniform1(locs.light_type, lights.count, reinterpret_cast<int*>(&lights.types));

                int cur = 0;
                for (unsigned int i = 0; i < lights.count; i++)
                {
                    if (sceneLights[lights.light_index[i]]->IsShadowCaster())
                        sceneLights[lights.light_index[i]]->SetShadowTexture(locs.light_depthTextures, i);
                    cur++;

                    if (cur > MAX_LIGHTS)
                        break;
                }
                OPENGL_CHECK_FOR_ERRORS();

            }
        }
    }

    OPENGL_CHECK_FOR_ERRORS();
    params["time"] = UUniformParam((float)GetTickCount());

    if (params.size() > 0)
    {
        map<string, UUniformParam>::iterator i = params.begin();
        do
        {
            if ((*i).second.data_size == 1)
                render->CacheUniform1((*i).first, 1, (*i).second.data.v);
            if ((*i).second.data_size == 2)
                render->CacheUniform2((*i).first, 1, (*i).second.data.v);
            if ((*i).second.data_size == 3)
                render->CacheUniform3((*i).first, 1, (*i).second.data.v);
            if ((*i).second.data_size == 4)
                render->CacheUniform4((*i).first, 1, (*i).second.data.v);

            OPENGL_CHECK_FOR_ERRORS();

            i++;
        } while (i != params.end());
    }

    OPENGL_CHECK_FOR_ERRORS();

    if (type == URenderPass::Forward || type == URenderPass::Deffered || type == URenderPass::Normal)
    {
        static std::vector<unsigned int> keys(16);
        keys.clear();

        for (auto it = uniform_textures.begin(); it != uniform_textures.end(); ++it)
            keys.push_back(it->first);

        for each(auto key in keys)
        {
            render->BindTexture(uniform_textures[key], key);
            render->CacheUniform1(uniform_textures[key]->name, key);
        }

        keys.clear();

        for (auto it = uniform_cubemaps.begin(); it != uniform_cubemaps.end(); ++it)
            keys.push_back(it->first);

        for each(auto key in keys)
        {
            render->BindCubemap(uniform_cubemaps[key], key);
            render->CacheUniform1(uniform_cubemaps[key]->name, key);
        }

        OPENGL_CHECK_FOR_ERRORS();
    }

    render->DepthWrite(is_depth_write_enabled);
    render->BlendMode(blend_mode);
    render->DepthFunc(depth_func);
}

void UMaterial::UnbindUniformUnits() const
{
    auto render = URenderer::GetInstance();

    for (auto it = uniform_textures.begin(); it != uniform_textures.end(); ++it)
        render->UnbindTexture((*it).first);

    for (auto it = uniform_cubemaps.begin(); it != uniform_cubemaps.end(); ++it)
        render->UnbindTexture((*it).first);

    OPENGL_CHECK_FOR_ERRORS();
}

void UMaterial::Render(UShaderProgram* sp)
{
    //not implemented yet
}