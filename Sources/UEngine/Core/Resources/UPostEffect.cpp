#include "UPostEffect.h"

UPostEffect::UPostEffect(void)
{
}


UPostEffect::~UPostEffect(void)
{
}

void UPostEffect:: AddTexture(UTexture* tex, int channel)
{
    //material.ClearTextures();
    material.AddTexture(std::pair<UTexture*, int>(tex, channel));
}

void UPostEffect:: Render(URENDER_TYPE type){
    this->material.params["time"] = (float)GetTickCount();
    this->UMesh::Render(type);
}
void UPostEffect:: Update(double delta){
    this->UMesh::Update(delta);
}

void UPostEffect:: Free(){
    this->UMesh::Free();
}

bool UPostEffect:: Load(std:: string path)
{
    UXMLFile xml;
    try
    {
        xml.Load(path);
        this->name = xml.GetElement("/xml/post_effect/name/");

        material.SetShaderProgram(rf->Load(xml.GetElement("/xml/post_effect/shaders/vertex_path/"), xml.GetElement("/xml/post_effect/shaders/pixel_path/")), URENDER_FORWARD);

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

        m_vertices[0] = UVertex(vec3(-1.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),vec2(0.0f, 0.0f));
        m_vertices[1] = UVertex(vec3(1.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),vec2(1.0f, 0.0f));
        m_vertices[2] = UVertex(vec3(-1.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),vec2(0.0f, 1.0f));
        m_vertices[3] = UVertex(vec3(1.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),vec2(1.0f, 1.0f));

        char tex_buffer[UE_MAXCHAR];

        if(xml.isExistElement("/xml/post_effect/tex_num/"))
        {
            int tex_num = atoi(xml.GetElement("/xml/post_effect/tex_num/").c_str());

            for(int j = 0; j < tex_num; j++){
                memset(tex_buffer,'\0',255);    
                sprintf_s(tex_buffer,"%d",j);
                std::string current_tex ="/xml/post_effect/textures/tex_" + string(tex_buffer) + "/";

                auto tex = pair<UTexture*, unsigned int>( 
                    dynamic_cast<UTexture*>(rf->Load(xml.GetElement(current_tex + "path/"), URESOURCE_TEXTURE)), 
                    atoi(xml.GetElement(current_tex + "channel/").c_str()));
                tex.first->name = xml.GetElement(current_tex + "name/");
                material.AddTexture(tex);
            }
        }

        Initialize();

    }
    catch(exception e)
    {
        ULogger::GetInstance()->Message("Error to load post effect (xml): " + path, ULOG_MSG_ERROR, ULOG_OUT_MSG);
        return false;
    }
    return true;
}
