#pragma once
#include "URenderer.h"
#include "UVertex.h"
#include "UBuffer.h"

class UIndexBuffer;

// VBO for vertices
class UVertexBuffer : public UBuffer
{
	// Num vertices
	int num_vertices;
	// Pointer
	UVertex *vertices;
	//VAO
	UVertexArrayObject* vao;
	
	void ComputeTBN(UIndexBuffer* ib);
	
public:

	//get pointer
	void* GetPointer() {return (void*)vertices;}
	// get num
	int GetNum() { return num_vertices; }

	// Create dynamic array
	void Create(int num_vertices) {
		//Free();
		this->num_vertices = num_vertices;
		vertices = new UVertex[num_vertices]();
	}
	
	//VAO
	UVertexArrayObject* GetVAO() {
		return vao;
	}
	
	bool Initialize(UIndexBuffer* ib)
	{
		if(ib != NULL)
			ComputeTBN(ib);
		return Initialize();
	}

	// Initialize in OpenGL driver
	bool Initialize(){	

		vao = new UVertexArrayObject();
		vao->Initialize();
		
		URenderer::GetInstance()->BindVAO(this);

		_id = -1;	
		_id = URenderer::GetInstance()->CreateVBO(this, UVBO_STATIC);
		
		return (_id != -1) && (vao->GetId() != -1);
	}
	// Delete all data
	void Free() {
		if(_id != -1)
			URenderer::GetInstance()->DeleteVBO(this);
		_id = -1;

		delete[] vertices;		
		delete vao;
	}

	UVertexBuffer(void);
	virtual ~UVertexBuffer(void);
};

