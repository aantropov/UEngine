#include "UMesh.h"
#include "../Basic/UScene.h"
#include"../Renderer/URenderManager.h"

UMesh::UMesh(void)
{
}

UMesh::~UMesh(void)
{
    Free();
}

void UMesh::AddToRenderQueue(URenderQueue& render_queue)
{
    render_queue.data[material.GetBlendMode()][material.GetQueue()].push_back(pair<mat4, UMesh*>(URenderer::GetInstance()->model_view, this));
}

void UMesh::InitializeMaterial(URenderPass type)
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

    InitializeMaterial(URenderPass::Normal);
    InitializeMaterial(URenderPass::Depth);
    InitializeMaterial(URenderPass::Forward);
    InitializeMaterial(URenderPass::Deffered);

    OPENGL_CHECK_FOR_ERRORS();
}

void UMesh::Render(URenderPass type, int light_index)
{
    if (type == URenderPass::DepthShadow && !material.IsShadowCaster())
        return;

    material.Render(type, light_index);
    URenderer::GetInstance()->BindVAO(&vb);
    URenderer::GetInstance()->BindVBO(&ib);
    URenderer::GetInstance()->DrawBuffer(&ib);
}

sphere UMesh::GetBounds() const { return boundSphere; }
void UMesh::ComputeBoundSphere() { boundSphere = vb.ComputeBoundSphere(); }