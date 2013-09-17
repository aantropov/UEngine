#pragma once

struct vec2;
struct vec3;
struct vec4;
struct quat;
struct mat2;
struct mat3;
struct mat4;
struct sphere;

struct frustum
{
    float plane[6][4];
    void extract(mat4 modl, mat4 proj);
};

bool IsPointInFrustum (vec3 point, const frustum fr);
bool IsSphereInFrustum(sphere sph, const frustum fr);
bool IsCubeInFrustum(const vec3 center, const float size, const frustum fr);