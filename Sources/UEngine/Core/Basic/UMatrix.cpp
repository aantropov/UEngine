#include "UMatrix.h"
#include "..\Renderer\URenderer.h"

UMatrix::UMatrix(void)
{
    m = mat4_identity;
}

UMatrix::~UMatrix(void)
{
}

void UMatrix::Set(mat4 m1)
{
    m = m1; 
}

mat4 UMatrix::Get()
{ 
    return m; 
}

void UMatrix::Set()
{
    URenderer::GetInstance()->modelView *= m;
}

void UMatrix::Render()
{
    URenderer::GetInstance()->modelView *= m;
}    

void UMatrix::Update()
{
}
