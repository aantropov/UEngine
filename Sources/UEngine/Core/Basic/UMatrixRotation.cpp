#include "UMatrixRotation.h"


UMatrixRotation::UMatrixRotation(vec3 a)
{
    ::UMatrix();

    haste.set(a);
    currentAngles = vec3_zero;
}


UMatrixRotation::~UMatrixRotation(void)
{
}
