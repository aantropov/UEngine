#pragma once
#include "..\math\umath.h"
#include "..\Utils\ExportDef.h"

class UCamera
{
    vec3 position;
    vec3 rotation;
    mat4 projection;

public:

    vec3 UENGINE_DECLSPEC GetPosition();
    vec3 UENGINE_DECLSPEC GetRotation();

    void UENGINE_DECLSPEC SetPosition(vec3 p) { position = p; }
    void UENGINE_DECLSPEC SetRotation(vec3 r) { rotation = r; }
    
    mat4 UENGINE_DECLSPEC GetProjection();
    mat4 UENGINE_DECLSPEC GetView();

    void UENGINE_DECLSPEC Create(float x, float y, float z);
    void UENGINE_DECLSPEC Perspective(float fov, float aspect, float zNear, float zFar);
    void UENGINE_DECLSPEC Ortho(float left, float right, float bottom, float top, float zNear, float zFar);
    void UENGINE_DECLSPEC LookAt(const vec3 &position, const vec3 &center, const vec3 &up);        
    void UENGINE_DECLSPEC Rotate(float x, float y, float z);
    void UENGINE_DECLSPEC Move( float x, float y, float z);
        
    UENGINE_DECLSPEC UCamera(void);
    UENGINE_DECLSPEC ~UCamera(void);
};

