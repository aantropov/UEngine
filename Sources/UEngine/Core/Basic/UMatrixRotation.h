#pragma once
#include "umatrix.h"

//It is just for test. Simple Matrix.
class UMatrixRotation :
    public UMatrix
{
    vec3 haste;
    vec3 currentAngles;

public:

    void Update(){
        currentAngles += haste;
        m = GLRotation(currentAngles.x, currentAngles.y, currentAngles.z);
    }
    UMatrixRotation(vec3 haste);
    ~UMatrixRotation(void);
};

