#include "UMesh.h"
#include "../Basic/UScene.h"

UMesh::UMesh(void)
{
}

UMesh::~UMesh(void)
{
    Free();
}

void UMesh::AddToRenderQueue(map<int, vector<UMesh*>>& render_queue)
{
    URenderer::GetInstance()->PushModelMatrix();
    m.Set();
    render_queue[material.GetQueue()].push_back(this);
    URenderer::GetInstance()->PopModelMatrix();
}

void UMesh::InitializeMaterial(URENDER_PASS type)
{
    if (material.GetShaderProgram(type) == nullptr)
        return;

    URenderer::GetInstance()->SetShaderProgram(material.GetShaderProgram(type));

    locations[type].position = glGetAttribLocation(material.GetShaderProgram(type)->GetId(), "position");
    locations[type].normal = glGetAttribLocation(material.GetShaderProgram(type)->GetId(), "normal");
    locations[type].texture_coord = glGetAttribLocation(material.GetShaderProgram(type)->GetId(), "texcoord");
    locations[type].binormal = glGetAttribLocation(material.GetShaderProgram(type)->GetId(), "binormal");
    locations[type].bone_indices = glGetAttribLocation(material.GetShaderProgram(type)->GetId(), "bone_indices");
    locations[type].bone_weights = glGetAttribLocation(material.GetShaderProgram(type)->GetId(), "bone_weights");


    if (locations[type].position != -1)
    {
        OPENGL_CALL(glVertexAttribPointer(locations[type].position, 3, GL_FLOAT, GL_FALSE, sizeof(UVertex), (const GLvoid*)0));
        OPENGL_CALL(glEnableVertexAttribArray(locations[type].position));
    }

    if (locations[type].normal != -1)
    {
        OPENGL_CALL(glVertexAttribPointer(locations[type].normal, 3, GL_FLOAT, GL_FALSE, sizeof(UVertex), (const GLvoid*)(sizeof(vec3))));
        OPENGL_CALL(glEnableVertexAttribArray(locations[type].normal));
    }

    if (locations[type].texture_coord != -1)
    {
        OPENGL_CALL(glVertexAttribPointer(locations[type].texture_coord, 2, GL_FLOAT, GL_FALSE, sizeof(UVertex), (const GLvoid*)(2 * sizeof(vec3))));
        OPENGL_CALL(glEnableVertexAttribArray(locations[type].texture_coord));
    }

    if (locations[type].binormal != -1)
    {
        OPENGL_CALL(glVertexAttribPointer(locations[type].binormal, 3, GL_FLOAT, GL_FALSE, sizeof(UVertex), (const GLvoid*)(2 * sizeof(vec3) + sizeof(vec2))));
        OPENGL_CALL(glEnableVertexAttribArray(locations[type].binormal));
    }

    if (locations[type].bone_indices != -1)
    {
        OPENGL_CALL(glVertexAttribPointer(locations[type].bone_indices, 4, GL_FLOAT, GL_FALSE, sizeof(UVertex), (const GLvoid*)(3 * sizeof(vec3) + sizeof(vec2))));
        OPENGL_CALL(glEnableVertexAttribArray(locations[type].bone_indices));
    }

    if (locations[type].bone_weights != -1)
    {
        OPENGL_CALL(glVertexAttribPointer(locations[type].bone_weights, 4, GL_FLOAT, GL_FALSE, sizeof(UVertex), (const GLvoid*)(3 * sizeof(vec3) + sizeof(vec2) + sizeof(vec4))));
        OPENGL_CALL(glEnableVertexAttribArray(locations[type].bone_weights));
    }
}

void UMesh::Initialize()
{
    ComputeBoundSphere();
    InitializeBuffers();

    auto renderer = URenderer::GetInstance();

    renderer->BindVBO(&vb);
    renderer->BindVBO(&ib);

    InitializeMaterial(URENDER_PASS_NORMAL);
    InitializeMaterial(URENDER_PASS_DEPTH);
    InitializeMaterial(URENDER_PASS_FORWARD);
    InitializeMaterial(URENDER_PASS_DEFFERED);

    OPENGL_CHECK_FOR_ERRORS();
}

void UMesh::Render(UMaterial *mat)
{
    URenderer::GetInstance()->PushModelMatrix();

    m.Set();
    mat->Render(URENDER_PASS_FORWARD);

    URenderer::GetInstance()->BindVAO(&vb);
    URenderer::GetInstance()->BindVBO(&ib);
    URenderer::GetInstance()->DrawBuffer(&ib);
    URenderer::GetInstance()->PopModelMatrix();
}

void UMesh::Render(URENDER_PASS type, int light_index)
{
    if (type == URENDER_PASS_DEPTH_SHADOW && !material.IsShadowCaster())
        return;

    URenderer::GetInstance()->PushModelMatrix();
    m.Set();
    material.Render(type, light_index);
    URenderer::GetInstance()->BindVAO(&vb);
    URenderer::GetInstance()->BindVBO(&ib);
    URenderer::GetInstance()->DrawBuffer(&ib);
    URenderer::GetInstance()->PopModelMatrix();
}

sphere UMesh::GetBounds() const { return boundSphere; }
void UMesh::ComputeBoundSphere() { boundSphere = vb.ComputeBoundSphere(); }