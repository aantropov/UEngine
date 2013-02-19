#pragma once
#include "URenderer.h"
#include "UBuffer.h"
// VBO for index buffrt
class UIndexBuffer : public UBuffer
{
	//Dynamic array
	unsigned int num_indices;	
	unsigned int *indices;
	
public:
	
	void* GetPointer() {return (void*)indices;}
	int GetNum() { return num_indices; }
	
	//Create array
	void Create(int num_faces) {
		Free();
		num_indices = num_faces*3;
		indices = new unsigned int[num_indices]();
	}
	
	// Init in OpenGL Driver
	bool Initialize(){		
		_id = -1;
		_id = URenderer::GetInstance()->CreateVBO(this, UVBO_STATIC );		
		return (_id != -1);
	}

	// Free resources
	void Free() {
		if(_id != -1)
			URenderer::GetInstance()->DeleteVBO(this);	
		_id = -1;
		delete[] indices;
	}
	
	UIndexBuffer(void);
	virtual ~UIndexBuffer(void);
};

