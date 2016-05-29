#include "UIndexBuffer.h"

UIndexBuffer::UIndexBuffer(void)
{
    state = UBufferUsage::Static;
    indices = NULL;
}

UIndexBuffer::~UIndexBuffer(void)
{
}
