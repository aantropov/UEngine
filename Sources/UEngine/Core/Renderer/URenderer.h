#pragma once
#include "UWindow.h"
#include "..\Utils\UConfig.h"
#include "..\Basic\USingleton.hpp"
#include "..\Basic\uscene.h"
#include "..\math\UMath.h"
#include "UVertex.h"
#include "UVertexArrayObject.h"
#include "..\Basic\UCamera.h"
#include "UBuffer.h"
#include "UShaderProgram.h"
#include "..\Utils\ExportDef.h"
#include "..\Math\UMath.h"
#include <unordered_map>
#include <map>

class UVertexBuffer;
class UIndexBuffer;
class UShader;
class UShaderProgram;
class UTexture;
class UScene;
class UFrameBufferObject;
class UCubemap;

class URenderer : public USingleton <URenderer>
{
    UWindow window;

    UScene* current_scene;
    std::vector<mat4> model_view_matrix_stack;

    // Cache
    unsigned int previousVAO;
    unsigned int previousIB;

    UShaderProgram *shader_program;

    map<unsigned int, unordered_map<string, unsigned int>> uniforms_ñache;
    map<unsigned int, unsigned int> tex_ñhannels_ñache;

    int CacheUniformLocation(string name);
    int CacheUniformLocation(string name, UShaderProgram *sh);

    float shadow_bias;

    bool InitExtensions();
    bool SetVerticalSynchronization(bool enabled);
    void PrintDebugInfo();

public:

    unsigned int draw_ñalls;
    float GetShadowBias() const { return shadow_bias; }
    std::string GetShaderVersion() const { return "#version 330 core"; }

    void UENGINE_DECLSPEC SetCurrentScene(UScene* current_scene);
    UScene UENGINE_DECLSPEC *GetCurrentScene();

    static URenderer UENGINE_DECLSPEC *GetInstance();

    URenderer();
    ~URenderer();

    UCamera current_camera;
    UCamera main_ñamera;

    bool Initialize();

    HWND GetHWND() { return window.GetHWND(); }

    int GetWidth() { return window.GetWidth(); }
    int GetHeight() { return window.GetHeight(); }

    void Release();

    //Operations with Stack
    mat4 model_view;
    void PushModelMatrix();
    void PopModelMatrix();

    //Camera
    UCamera GetCurrentCamera() const;
    void SetCurrentCamera(UCamera cam);
    void SetupCameraForShaderProgram(UShaderProgram *shd, mat4 &model_view);
    void SetupCameraForShaderProgram(UCamera *cam, UShaderProgram *shd, const mat4 &model_view);
    void SetupCameraLightForShaderProgram(UCamera &camera);

    //Texture & Cubemap
    int CreateCubemap(UCubemap *tex) const;
    void BindCubemap(UCubemap *tex, unsigned int channel);
    void DeleteCubemap(UCubemap *tex) const;

    int CreateTexture(UTexture *tex) const;
    void BindTexture(UTexture *tex);
    void DeleteTexture(UTexture *tex) const;
    void BindTexture(UTexture *tex, unsigned int channel);
    void UnbindTexture(unsigned int channel);

    //VBO
    void BindVBO(UVertexBuffer *vb)  const;
    void BindVBO(UIndexBuffer *ib);
    void UnbindVBO(bool is_vertex_buffer)  const;

    int CreateVBO(UVertexBuffer *vb, UBufferUsage state)  const;
    int CreateVBO(UIndexBuffer *ib, UBufferUsage state)  const;
    void DeleteVBO(UBuffer *vb)  const;

    //FBO
    int CreateFBO()  const;
    void DeleteFBO(UFrameBufferObject *fb)  const;
    void BindFBO(UFrameBufferObject *fb)  const;
    void UnbindFBO()  const;

    //VAO
    void BindVAO(UVertexBuffer *vb);
    void UnbindVAO()  const;

    int CreateVAO()  const;
    void DeleteVAO(UVertexArrayObject *vao)  const;

    //Shaders    
    int  CompileShader(const std::string* source, UShaderType st, std::vector<std::string> defines)  const;
    void DeleteShader(UShader* shd)  const;

    int CreateShaderProgram(UShader *vertex_sh, UShader *pixel_sh)  const;
    void SetShaderProgram(UShaderProgram *sh);
    void DeleteShaderProgram(UShaderProgram *sh);

    void CacheReleaseUniformLocation(UShaderProgram* sh);
    void CacheUniform4(UShaderProgram *sh, std::string name, unsigned int num, float *variable);
    void CacheUniform4(std::string name, unsigned int num, float *variable);

    void CacheUniform1(UShaderProgram *sh, std::string name, unsigned int num, float *variable);
    void CacheUniform1(std::string name, unsigned int num, float *variable);
    void CacheUniform1(std::string name, int value);

    void CacheUniform2(std::string name, unsigned int num, float *variable);
    void CacheUniform2(UShaderProgram *sh, std::string name, unsigned int num, float *variable);

    void CacheUniform3(UShaderProgram *sh, std::string name, unsigned int num, float *variable);
    void CacheUniform3(std::string name, unsigned int num, float *variable);

    void CacheUniformMatrix4(std::string name, unsigned int num, float *variable);
    void CacheUniformMatrix3(std::string name, unsigned int num, float *variable);

    void CacheUniformMatrix4(UShaderProgram *sh, std::string name, unsigned int num, float *variable);
    void CacheUniformMatrix3(UShaderProgram *sh, std::string name, unsigned int num, float *variable);

    void Uniform4(const unsigned int location, const unsigned int num, const float *variable) const;
    void Uniform1(const unsigned int location, const unsigned int num, const float *variable) const;
    void Uniform1(const unsigned int location, const int value) const;
    void Uniform1(const unsigned int location, const unsigned int num, const int* value) const;
    void Uniform2(const unsigned int location, const unsigned int num, const float *variable) const;
    void Uniform3(const unsigned int location, const unsigned int num, const float *variable) const;
    void UniformMatrix4(const unsigned int location, const unsigned int num, const float *variable) const;
    void UniformMatrix3(const unsigned int location, const unsigned int num, const float *variable) const;

    void DepthWrite(bool value) const;

    void DrawSegment(const vec3& p1, const vec3& p2, const vec3& color);
    void DrawTransform(::transform xf);
    void DrawSolidPolygon(const UVertex* vertices, int vertexCount, const vec4 color);

    void DrawBuffer(UVertexBuffer *vb);
    void DrawBuffer(UIndexBuffer* ib);

    void DrawDebugLine(vec3 start, vec3 end, vec3 color);
};