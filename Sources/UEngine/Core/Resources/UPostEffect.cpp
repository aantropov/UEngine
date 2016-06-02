#include "UPostEffect.h"

UPostEffect::UPostEffect(void)
{
}


UPostEffect::~UPostEffect(void)
{
}

void UPostEffect::ClearUniformUnits()
{
    material.ClearUniformUnits();
}

void UPostEffect::AddTexture(UTexture* tex, int channel)
{
    //material.ClearTextures();
    material.AddUniformUnit(channel, tex);
}

void UPostEffect::Render(URenderPass type, int light_index)
{
    this->material.params["time"] = (float)GetTickCount();
    this->UMesh::Render(type, light_index);
}

void UPostEffect::Update(double delta)
{
    this->UMesh::Update(delta);
}

void UPostEffect::Free() {
    this->UMesh::Free();
}

bool UPostEffect::Load(UXMLFile& xml, std::string path)
{
    try
    {
        this->name = xml.GetElement(path + "post_effect/name/");

        UShaderProgram* shader_program = (UShaderProgram*)rf->Create(xml.GetPath() + " shader program", UResourceType::ShaderProgram);
        shader_program->Load(xml, path + "post_effect/");

        material.SetShaderProgram(shader_program, URenderPass::Forward);
        material.SetShaderProgram(shader_program, URenderPass::Deffered);

        ib.Create(2);

        unsigned int *m_indices = reinterpret_cast<unsigned int*>(ib.GetPointer());

        m_indices[0] = 0;
        m_indices[1] = 1;
        m_indices[2] = 2;

        m_indices[3] = 2;
        m_indices[4] = 1;
        m_indices[5] = 3;

        vb.Create(4);
        UVertex *m_vertices = reinterpret_cast<UVertex*>(vb.GetPointer());

        m_vertices[0] = UVertex(vec3(-1.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f));
        m_vertices[1] = UVertex(vec3(1.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f));
        m_vertices[2] = UVertex(vec3(-1.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f));
        m_vertices[3] = UVertex(vec3(1.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f));

        char tex_buffer[UE_MAXCHAR];

        if (xml.isExistElement(path + "post_effect/tex_num/"))
        {
            int tex_num = atoi(xml.GetElement(path + "post_effect/tex_num/").c_str());

            for (int j = 0; j < tex_num; j++) {
                memset(tex_buffer, '\0', 255);
                sprintf_s(tex_buffer, "%d", j);
                std::string current_tex = path + "post_effect/textures/tex_" + string(tex_buffer) + "/";

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
                
                material.AddUniformUnit(tex.second, tex.first);
            }
        }

        vb.SetState(UBufferUsage::Dynamic);
        Initialize();

    }
    catch (exception e)
    {
        ULogger::GetInstance()->Message("Error to load post effect (xml): " + path, ULogType::Error, ULogTarget::MsgBox);
        return false;
    }
    return true;
}
