#include "UCamera.h"

vec3 UCamera::GetPosition() const { return position; }
vec3 UCamera::GetRotation() const { return rotation; }

mat4 UCamera::GetProjection() const { return projection; }
mat4 UCamera::GetView() const { return GLRotation(rotation.x, rotation.y, rotation.z) * GLTranslation(-position); }

void UCamera::UpdateFrustum()
{
    frustumPlanes.extract(GetView(), GetProjection());
}

void UCamera::Create(float x, float y, float z)
{
    position   = vec3(x, y, z);
    projection = mat4_identity;
}

void UCamera::Perspective(float fov, float aspect, float zNear, float zFar)
{
    projection = GLPerspective(fov, aspect, zNear, zFar);
}

void UCamera::Ortho(float left, float right, float bottom, float top, float zNear, float zFar)
{
    projection = GLOrthographic(left, right, bottom, top, zNear, zFar);
}

void UCamera::Rotate(float x, float y, float z)
{
    rotation += vec3(x, y, z);
}

void UCamera::Move( float x, float y, float z)
{
    vec3 move = transpose(mat3(GLRotation(rotation.x,rotation.y,rotation.z))) * vec3(x, y, z);
    //vec3 move = vec3(x,y,z);
    position += move;
}

void UCamera::LookAt(const vec3 &position, const vec3 &center, const vec3 &up)
{
    rotation = GLToEuler(GLLookAt(position, center, up));
    this->position = position;
}
    
UCamera::UCamera(void){ rotation = vec3_zero; }
UCamera:: ~UCamera(void){}
