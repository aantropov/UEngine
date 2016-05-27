#pragma once
#include "..\math\umath.h"
#include "..\Utils\ExportDef.h"

class UCamera
{
    float z_near;
    float z_far;

    vec3 position;
    vec3 rotation;
    mat4 projection;

    frustum frustum_planes;

public:

    void UENGINE_DECLSPEC UpdateFrustum();

    vec3 UENGINE_DECLSPEC GetPosition() const;
    vec3 UENGINE_DECLSPEC GetRotation()  const;
    const frustum UENGINE_DECLSPEC GetFrustum() const { return frustum_planes; }

    const float UENGINE_DECLSPEC GetZNear() const { return z_near; }
    const float UENGINE_DECLSPEC GetZFar() const { return z_far; }

    void UENGINE_DECLSPEC SetPosition(vec3 p) { position = p; }
    void UENGINE_DECLSPEC SetRotation(vec3 r) { rotation = r; }
    
    mat4 UENGINE_DECLSPEC GetProjection() const;
    mat4 UENGINE_DECLSPEC GetView() const;

    void UENGINE_DECLSPEC Create(float x, float y, float z);
    void UENGINE_DECLSPEC Perspective(float fov, float aspect, float zNear, float zFar);
    void UENGINE_DECLSPEC Ortho(float left, float right, float bottom, float top, float zNear, float zFar);
    void UENGINE_DECLSPEC LookAt(const vec3 &position, const vec3 &center, const vec3 &up);        
    void UENGINE_DECLSPEC Rotate(float x, float y, float z);
    void UENGINE_DECLSPEC Move( float x, float y, float z);
        
    UENGINE_DECLSPEC UCamera(void);
    UENGINE_DECLSPEC ~UCamera(void);
};

