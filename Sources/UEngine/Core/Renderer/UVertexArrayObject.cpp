#include "UVertexArrayObject.h"
#include "URenderer.h"
#include "UVertex.h"
#include "UBuffer.h"

bool UVertexArrayObject:: Initialize()
{
    _id = -1;    
    _id = URenderer::GetInstance()->CreateVAO();
    return (_id != -1);
}

void UVertexArrayObject::  Free()
{
    if(_id != -1)
        URenderer::GetInstance()->DeleteVAO(this);
    _id = -1;
}