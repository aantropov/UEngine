#pragma once
#include "vec3.h"

struct sphere
{
    vec3 center;
    float radius;

    sphere(): center(0.0f, 0.0f, 0.0f), radius(0.0f) {}
    sphere(vec3 c, float r): center(c), radius(r) {}
};