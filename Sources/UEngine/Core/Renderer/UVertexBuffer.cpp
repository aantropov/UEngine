#include "UVertexBuffer.h"
#include "UIndexBuffer.h"


UVertexBuffer::UVertexBuffer(void)
{
	vertices = NULL;
}


UVertexBuffer::~UVertexBuffer(void)
{
	Free();
}

void UVertexBuffer:: ComputeTBN(UIndexBuffer* ib)
{
	unsigned int *data = (unsigned int*)ib->GetPointer();
	unsigned int iterator = 0;
	
	for(int i = 0; i < ib->GetNum()/3 ; i ++){
			
		vertices[data[iterator]].SetBinormal(
			vertextangentspace(vertices[data[iterator]].GetPosition(), 
						   vertices[data[iterator+1]].GetPosition(),
						   vertices[data[iterator+2]].GetPosition(),
						   vertices[data[iterator]].GetTexcoord(), 
						   vertices[data[iterator+1]].GetTexcoord(),
						   vertices[data[iterator+2]].GetTexcoord()));
		vertices[data[iterator+1]].SetBinormal(
			vertextangentspace(vertices[data[iterator+1]].GetPosition(), 
						   vertices[data[iterator+2]].GetPosition(),
						   vertices[data[iterator]].GetPosition(),
						   vertices[data[iterator+1]].GetTexcoord(), 
						   vertices[data[iterator+2]].GetTexcoord(),
						   vertices[data[iterator]].GetTexcoord()));
		vertices[data[iterator+2]].SetBinormal(
			vertextangentspace(vertices[data[iterator+2]].GetPosition(), 
						   vertices[data[iterator]].GetPosition(),
						   vertices[data[iterator+1]].GetPosition(),
						   vertices[data[iterator+2]].GetTexcoord(), 
						   vertices[data[iterator]].GetTexcoord(),
						   vertices[data[iterator+1]].GetTexcoord()));
		iterator += 3;							   
	}
}