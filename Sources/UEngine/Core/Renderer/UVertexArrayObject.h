#pragma once
#include "UGLObject.h"

// VBO for vertices
class UVertexArrayObject: public UGLObject
{
	
public:
	
	// Initialize in OpenGL driver
	bool Initialize();

	// Delete all data
	void Free();

	UVertexArrayObject(void){ _id = -1; };
	virtual ~UVertexArrayObject(void){Free();};
};

