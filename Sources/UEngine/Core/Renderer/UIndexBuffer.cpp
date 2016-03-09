#include "UIndexBuffer.h"

UIndexBuffer::UIndexBuffer(void)
{
    state = UVBO_STATIC;
    indices = NULL;
}

UIndexBuffer::~UIndexBuffer(void)
{
}
