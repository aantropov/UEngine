#pragma once
#include "URenderer.h"
#include "UVertex.h"
#include "UBuffer.h"

class UIndexBuffer;

class UVertexBuffer : public UBuffer
{    
    int num_vertices;    
    UVertex *vertices;    
    UVertexArrayObject* vao;
    
    void ComputeTBN(UIndexBuffer* ib);
    
public:

    sphere ComputeBoundSphere()
    {
        unsigned int index = 0;
        for(int i = 0; i < num_vertices; i++)
            if(length(vertices[index].GetPosition()) < length(vertices[i].GetPosition()))
                index = i;
        return sphere(vec3_zero, length(vertices[index].GetPosition()));
    }
    
	void* GetPointer()
	{
		return (void*)vertices;
	}

    int GetNum() 
	{ 
		return num_vertices; 
	}

    void Create(int num_vertices) 
	{
        this->num_vertices = num_vertices;
        vertices = new UVertex[num_vertices]();
    }
    
    //VAO
    UVertexArrayObject* GetVAO()
	{
        return vao;
    }
    
    bool Initialize(UIndexBuffer* ib)
    {
        if(ib != NULL)
            ComputeTBN(ib);
        return Initialize();
    }
        
    bool Initialize()
	{    
        vao = new UVertexArrayObject();
        vao->Initialize();
        
        URenderer::GetInstance()->BindVAO(this);
        
		_id = -1;    
        _id = URenderer::GetInstance()->CreateVBO(this, UVBO_STATIC);
        
        return (_id != -1) && (vao->GetId() != -1);
    }
    
    void Free() 
	{
        if(_id != -1)
            URenderer::GetInstance()->DeleteVBO(this);
        _id = -1;

        delete[] vertices;        
        delete vao;
    }

    void* Lock()
    {
        glBindBuffer(GL_ARRAY_BUFFER, UGLObject::_id);
        glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(UVertex), 0, GL_STREAM_DRAW_ARB);
        UVertex* pBuffer = (UVertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY_ARB);            
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return pBuffer;
    }

    void Unlock()
    {        
        glBindBuffer(GL_ARRAY_BUFFER, UGLObject::_id);
        GLboolean result = glUnmapBuffer(GL_ARRAY_BUFFER);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    UVertexBuffer(void);
    virtual ~UVertexBuffer(void);
};