#include "UModel.h"
#include "..\Utils\utils.h"
#include "UTexture.h"
#include "stdio.h"
#include "..\Basic\UScene.h"
#include "..\Renderer\URenderManager.h"

void UModel::AddToRenderQueue(URenderQueue& render_queue)
{
    URenderer::GetInstance()->PushModelMatrix();
    for (int i = mesh_num - 1; i >= 0; i--)
    {
        if (count_playable_animations > 0)
        {
            meshes[i]->material.SetSkinningMatrixes(bone_matrixes, current_frame.bones.size());
        }
        meshes[i]->AddToRenderQueue(render_queue);
    }
    URenderer::GetInstance()->PopModelMatrix();
}

void UModel::Render(URenderPass type, int light_index)
{
    URenderer::GetInstance()->PushModelMatrix();
    for (int i = mesh_num - 1; i >= 0; i--)
    {
        if (count_playable_animations > 0)
        {
            meshes[i]->material.SetSkinningMatrixes(bone_matrixes, current_frame.bones.size());
        }
        meshes[i]->Render(type, light_index);
    }
    URenderer::GetInstance()->PopModelMatrix();
}

void UModel::Update(double delta)
{
    UComponent::Update(delta);
    for (unsigned int i = 0; i < mesh_num; i++)
        meshes[i]->Update(delta);
}

void UModel::Free()
{
    for (unsigned int i = 0; i < mesh_num; i++)
        delete meshes[i];
    meshes.clear();
    mesh_num = 0;
}

bool UModel::Load(UXMLFile& xml, std::string path)
{
    char buffer[UE_MAXCHAR];
    char tex_buffer[UE_MAXCHAR];

    try {

        this->name = xml.GetElement(path + "model/name/");

        if (xml.isExistElement(path + "model/upm/"))
        {
            int data_size = 0;
            int i_buffer = 0;

            FILE* file = NULL;
            fopen_s(&file, xml.GetElement(path + "model/upm/").c_str(), "rb");

            if (file == NULL)
            {
                ULogger::GetInstance()->Message("Cannot find file: " + xml.GetElement(path + "model/upm/"));
                throw;
            }

            fread(&i_buffer, 4, 1, file);
            if (i_buffer != 111)
            {
                ULogger::GetInstance()->Message("Wrong upm format: " + xml.GetElement(path + "model/upm/"));
                throw;
            }

            fread(&i_buffer, 4, 1, file);

            this->mesh_num = i_buffer;

            for (unsigned int i = 0; i < mesh_num; i++)
            {
                UMesh* mesh = new UMesh();
                fread(&data_size, 4, 1, file);

                fread(&i_buffer, 4, 1, file);
                mesh->vb.Create(i_buffer);

                fread(&i_buffer, 4, 1, file);
                mesh->ib.Create(i_buffer);

                char c_buffer[MAXCHAR];
                memset(buffer, 0, MAXCHAR);

                int count = 0;
                do
                {
                    fread(&c_buffer[count], 1, 1, file);
                    count++;
                } while (c_buffer[count - 1] != '\0');

                mesh->name = string(c_buffer);

                bool finded = false;
                std::string current_mesh;
                int xml_mesh_num = atoi(xml.GetElement(path + "model/mesh_num/").c_str());
                for (int j = 0; j < xml_mesh_num; j++)
                {
                    memset(buffer, '\0', 255);
                    sprintf_s(buffer, "%d", j);
                    current_mesh = path + "model/mesh_" + string(buffer) + "/";
                    if (xml.GetElement(current_mesh + "name/") == mesh->name)
                    {
                        finded = true;
                        break;
                    }
                }

                unsigned char ar, ab, ag;
                unsigned char dr, db, dg, da;
                unsigned char sr, sb, sg;
                unsigned char shin;
                unsigned char two_sides;

                fread(&dr, 1, 1, file);
                fread(&dg, 1, 1, file);
                fread(&db, 1, 1, file);
                fread(&da, 1, 1, file);

                fread(&two_sides, 1, 1, file);

                fread(&ar, 1, 1, file);
                fread(&ag, 1, 1, file);
                fread(&ab, 1, 1, file);

                fread(&sr, 1, 1, file);
                fread(&sg, 1, 1, file);
                fread(&sb, 1, 1, file);

                fread(&shin, 1, 1, file);

                mesh->material = UMaterial(
                    vec4(color(dr, dg, db, da)),
                    vec4(color(sr, sg, sb, 255)),
                    vec4_zero,
                    //vec4(color(ar, ag, ab, 255)),
                    ((float)shin) / 255.0f);

                unsigned int *m_indices = reinterpret_cast<unsigned int*>(mesh->ib.GetPointer());
                for (int j = 0; j < mesh->ib.GetNum(); j++)
                {
                    short ind;
                    fread(&ind, 2, 1, file);
                    m_indices[j] = ind;
                }
                //fread(m_indices, 2, mesh->ib.GetNum(), file);

                UVertex *m_vertices = reinterpret_cast<UVertex*>(mesh->vb.GetPointer());
                for (int j = 0; j < mesh->vb.GetNum(); j++)
                {
                    float x, y, z;
                    float nx, ny, nz;
                    float u, v;
                    vec4 ind;
                    vec4 weight;

                    fread(&x, 4, 1, file);
                    fread(&y, 4, 1, file);
                    fread(&z, 4, 1, file);

                    fread(&nx, 4, 1, file);
                    fread(&ny, 4, 1, file);
                    fread(&nz, 4, 1, file);

                    fread(&u, 4, 1, file);
                    fread(&v, 4, 1, file);

                    fread(ind.v, 4, 4, file);
                    fread(weight.v, 4, 4, file);

                    m_vertices[j] = UVertex(vec3(x, y, z), normalize(vec3(nx, ny, nz)), vec2(u, v));

                    m_vertices[j].SetBoneIndex(ind);
                    m_vertices[j].SetBoneWeight(normalize(weight));
                }

                if (xml.isExistElement(path + "model/common_material/"))
                {
                    mesh->material = *dynamic_cast<UMaterial*>(rf->Create(UResourceType::Material));
                    mesh->material.LoadFromFile(xml.GetElement(path + "model/common_material/"));
                }

                if (xml.isExistElement(path + "model/common_shaders/forward/"))
                    mesh->material.SetShaderProgram((UShaderProgram*)rf->Load(xml.GetElement(path + "model/common_shaders/forward/"), UResourceType::ShaderProgram), URenderPass::Forward);

                if (xml.isExistElement(path + "model/common_shaders/normal"))
                    mesh->material.SetShaderProgram((UShaderProgram*)rf->Load(xml.GetElement(path + "model/common_shaders/normal/"), UResourceType::ShaderProgram), URenderPass::Normal);

                if (xml.isExistElement(path + "model/common_shaders/depth/"))
                    mesh->material.SetShaderProgram((UShaderProgram*)rf->Load(xml.GetElement(path + "model/common_shaders/depth/"), UResourceType::ShaderProgram), URenderPass::Depth);

                if (xml.isExistElement(path + "model/common_shaders/deffered/"))
                    mesh->material.SetShaderProgram((UShaderProgram*)rf->Load(xml.GetElement(path + "model/common_shaders/deffered/"), UResourceType::ShaderProgram), URenderPass::Deffered);

                if (xml.isExistElement(path + "model/tex_num/"))
                {
                    int tex_num = atoi(xml.GetElement(path + "model/tex_num/").c_str());
                    for (int j = 0; j < tex_num; j++)
                    {
                        memset(tex_buffer, '\0', 255);
                        sprintf_s(tex_buffer, "%d", j);
                        std::string current_tex = path + "model/common_textures/tex_" + string(tex_buffer) + "/";

                        auto tex = pair<UTexture*, unsigned int>(
                            dynamic_cast<UTexture*>(rf->Load(xml.GetElement(current_tex + "path/"), UResourceType::Texture)),
                            atoi(xml.GetElement(current_tex + "channel/").c_str()));

                        tex.first->name = xml.GetElement(current_tex + "name/");
                        mesh->material.AddUniformUnit(tex.second, tex.first);                      
                    }
                }

                if (finded)
                {
                    if (xml.isExistElement(current_mesh + "material/"))
                    {
                        mesh->material = *dynamic_cast<UMaterial*>(rf->Create(UResourceType::Material));
                        mesh->material.LoadFromFile(xml.GetElement(current_mesh + "material/"));
                    }

                    if (xml.isExistElement(current_mesh + "tex_num/"))
                    {
                        int tex_num = atoi(xml.GetElement(current_mesh + "tex_num/").c_str());
                        for (int j = 0; j < tex_num; j++)
                        {
                            memset(tex_buffer, '\0', 255);
                            sprintf_s(tex_buffer, "%d", j);
                            std::string current_tex = current_mesh + "textures/tex_" + string(tex_buffer) + "/";

                            auto tex = pair<UTexture*, unsigned int>(
                                dynamic_cast<UTexture*>(rf->Load(xml.GetElement(current_tex + "path/"), UResourceType::Texture)),
                                atoi(xml.GetElement(current_tex + "channel/").c_str()));

                            tex.first->name = xml.GetElement(current_tex + "name/");

                            mesh->material.AddUniformUnit(tex.second, tex.first);
                        }
                    }
                }

                int anim_num = atoi(xml.GetElement(path + "model/anim_num/").c_str());
                if (anim_num > 0)
                {
                    for (int j = 0; j < anim_num; j++)
                    {
                        memset(tex_buffer, '\0', 255);
                        sprintf_s(tex_buffer, "%d", j);
                        std::string current_anim = path + "model/animations/animation_" + string(tex_buffer) + "/";

                        animations[xml.GetElement(current_anim + "name/")] = dynamic_cast<UAnimation*>(rf->Create(UResourceType::Animation));
                        animations[xml.GetElement(current_anim + "name/")]->LoadFromFile(xml.GetElement(current_anim + "path/"));
                    }
                }

                mesh->Initialize();
                meshes.push_back(mesh);
            }
            fclose(file);
        }

    }
    catch (exception e)
    {
        ULogger::GetInstance()->Message("Error to load model (xml): " + path, ULogType::Error, ULogTarget::MsgBox);
        return false;
    }
    return true;
}