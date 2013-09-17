#pragma once
#include "UVertexBuffer.h"
#include "UIndexBuffer.h"
#include "UShaderProgram.h"
#include "URenderer.h"
#include "..\Resources\UMaterial.h"
#include "..\Resources\UTexture.h"
#include "..\Basic\ULight.h"
#include "..\Basic\UMatrix.h"
#include "..\Basic\UIRenderable.h"
#include <map>

class UMesh: public UNode, public UIRenderable
{
    class UShaderParameters
    {
        public:

        int positionLocation;
        int normalLocation;
        int textureCoordLocation;
        int binormalLocation;
        int boneIndicesLocation;
        int boneWeightsLocation;
    };

    bool InitializeBuffers()
    {
        return ib.Initialize() & vb.Initialize(&ib);
    }
    
    map<URENDER_TYPE, UShaderParameters> parameters;
    sphere boundSphere;

public:
    
    UMatrix m;

    // Data    
    std::string name;
    UVertexBuffer vb;
    UIndexBuffer ib;
    UMaterial material;    

    void InitializeMaterial(URENDER_TYPE type)
    {            
        if(material.GetShaderProgram(type) == nullptr)
            return;

        URenderer::GetInstance()->SetShaderProgram(material.GetShaderProgram(type));
        
        parameters[type].positionLocation = glGetAttribLocation(material.GetShaderProgram(type)->GetId(), "position");
        parameters[type].normalLocation = glGetAttribLocation(material.GetShaderProgram(type)->GetId(), "normal");        
        parameters[type].textureCoordLocation = glGetAttribLocation(material.GetShaderProgram(type)->GetId(), "texcoord");
        parameters[type].binormalLocation = glGetAttribLocation(material.GetShaderProgram(type)->GetId(), "binormal");
        parameters[type].boneIndicesLocation = glGetAttribLocation(material.GetShaderProgram(type)->GetId(), "bone_indices");
        parameters[type].boneWeightsLocation = glGetAttribLocation(material.GetShaderProgram(type)->GetId(), "bone_weights");

    
        if (parameters[type].positionLocation != -1)
		{
            OPENGL_CALL(glVertexAttribPointer(parameters[type].positionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(UVertex), (const GLvoid*)0));
            OPENGL_CALL(glEnableVertexAttribArray(parameters[type].positionLocation));
        }

        if (parameters[type].normalLocation != -1)
		{
            OPENGL_CALL(glVertexAttribPointer(parameters[type].normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(UVertex), (const GLvoid*)(sizeof(vec3))));
            OPENGL_CALL(glEnableVertexAttribArray(parameters[type].normalLocation));
        }

        if (parameters[type].textureCoordLocation != -1)
		{
            OPENGL_CALL(glVertexAttribPointer(parameters[type].textureCoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(UVertex), (const GLvoid*)(2*sizeof(vec3))));
            OPENGL_CALL(glEnableVertexAttribArray(parameters[type].textureCoordLocation));
        }
        
        if (parameters[type].binormalLocation != -1)
		{
            OPENGL_CALL(glVertexAttribPointer(parameters[type].binormalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(UVertex), (const GLvoid*)(2*sizeof(vec3) + sizeof(vec2))));
            OPENGL_CALL(glEnableVertexAttribArray(parameters[type].binormalLocation));
        }

        if (parameters[type].boneIndicesLocation != -1)
		{
            OPENGL_CALL(glVertexAttribPointer(parameters[type].boneIndicesLocation, 4, GL_FLOAT, GL_FALSE, sizeof(UVertex), (const GLvoid*)(3*sizeof(vec3) + sizeof(vec2))));
            OPENGL_CALL(glEnableVertexAttribArray(parameters[type].boneIndicesLocation));
        }

        if (parameters[type].boneWeightsLocation != -1)
		{
            OPENGL_CALL(glVertexAttribPointer(parameters[type].boneWeightsLocation, 4, GL_FLOAT, GL_FALSE, sizeof(UVertex),  (const GLvoid*)(3*sizeof(vec3) + sizeof(vec2)+ sizeof(vec4))));
            OPENGL_CALL(glEnableVertexAttribArray(parameters[type].boneWeightsLocation));
        }
    }
    
    virtual void Initialize()
	{
        ComputeBoundSphere();
        InitializeBuffers();
		
		auto renderer = URenderer::GetInstance();
        
		renderer->BindVBO(&vb);
        renderer->BindVBO(&ib);

        InitializeMaterial(URENDER_NORMAL);
        InitializeMaterial(URENDER_DEPTH);
        InitializeMaterial(URENDER_FORWARD);
        InitializeMaterial(URENDER_DEFFERED);

        OPENGL_CHECK_FOR_ERRORS();
    }

    virtual void Render(UMaterial *mat)
    {
        URenderer::GetInstance()->PushModelMatrix();

        m.Set();
        mat->Render(URENDER_FORWARD);
        
        URenderer::GetInstance()->BindVAO(&vb);
        URenderer::GetInstance()->BindVBO(&ib);
        URenderer::GetInstance()->DrawBuffer(&ib);
        URenderer::GetInstance()->PopModelMatrix();    
    }

    virtual void Render(URENDER_TYPE type)
	{
        URenderer::GetInstance()->PushModelMatrix();
        m.Set();
        material.Render(type);
        URenderer::GetInstance()->BindVAO(&vb);
        URenderer::GetInstance()->BindVBO(&ib);
        URenderer::GetInstance()->DrawBuffer(&ib);
        URenderer::GetInstance()->PopModelMatrix();
    }
    
    sphere GetBounds() const { return boundSphere; }
    void ComputeBoundSphere() { boundSphere = vb.ComputeBoundSphere(); }

    void Free(){}

    UMesh(void);
    ~UMesh(void);
};

