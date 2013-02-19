#pragma once
#include "UGLObject.h"

// Base class for Index and Vertex buffers
class UBuffer: public UGLObject
{
public:

	// Get pointer to data in buffer
	virtual void* GetPointer() = 0;
	// Get count of elements in buffer
	virtual int GetNum() = 0;	
	// Create dynamic array
	virtual void Create(int num) = 0;	
	// Initialize buffer in Opengl
	virtual bool Initialize() = 0;
	// Free all data and deinitialise
	virtual void Free() = 0;
	
	UBuffer(void);
	virtual ~UBuffer(void);
};

