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

class URenderer : public USingleton < URenderer >
{
    UWindow uWnd;

    bool InitExtensions();
    bool SetVerticalSynchronization(bool enabled);

    void PrintDebugInfo();

    std::vector<mat4> modelViewMatrixStack;
    UScene* currentScene;

    UShaderProgram *shaderProgram;

    // Optimization
    unsigned int previousVAO;
    unsigned int previousIB;

    map<unsigned int, unordered_map<string, unsigned int>> uniformsCache;
    map<unsigned int, unsigned int> texChannelsCache;

    int CacheUniformLocation(string name);
    int CacheUniformLocation(string name, UShaderProgram *sh);

    float shadow_bias;
public:

    unsigned int drawCalls;
    float GetShadowBias() const { return shadow_bias; }
    std::string GetShaderVersion() const { return "#version 330 core"; }

    void UENGINE_DECLSPEC SetCurrentScene(UScene* currentScene);
    UScene UENGINE_DECLSPEC *GetCurrentScene();

    static URenderer UENGINE_DECLSPEC *GetInstance();

    URenderer();
    ~URenderer();

    UCamera currentCamera;
    UCamera mainCamera;

    bool Initialize();

    HWND GetHWND() { return uWnd.GetHWND(); }

    int GetWidth() { return uWnd.GetWidth(); }
    int GetHeight() { return uWnd.GetHeight(); }

    void Release();

    //Operations with Stack
    mat4 modelView;
    void PushModelMatrix();
    void PopModelMatrix();

    //Camera
    UCamera GetCurrentCamera();
    void SetCurrentCamera(UCamera cam);
    void SetupCameraForShaderProgram(UShaderProgram *shd, mat4 &model);
    void SetupCameraForShaderProgram(UCamera *cam, UShaderProgram *shd, const mat4 &model);
    void SetupCameraLightForShaderProgram(UCamera &camera);

    //Texture & Cubemap
    int CreateCubemap(UCubemap *tex);
    void BindCubemap(UCubemap *tex, unsigned int channel);
    void DeleteCubemap(UCubemap *tex);

    int CreateTexture(UTexture *tex);
    void BindTexture(UTexture *tex);
    void DeleteTexture(UTexture *tex);
    void BindTexture(UTexture *tex, unsigned int channel);

    //VBO
    void BindVBO(UVertexBuffer *vb);
    void BindVBO(UIndexBuffer *ib);
    void UnbindVBO(bool is_vertex_buffer);

    int CreateVBO(UVertexBuffer *vb, UVBO_DRAW state);
    int CreateVBO(UIndexBuffer *ib, UVBO_DRAW state);
    void DeleteVBO(UBuffer *vb);

    //FBO
    int CreateFBO();
    void DeleteFBO(UFrameBufferObject *fb);
    void BindFBO(UFrameBufferObject *fb);
    void UnbindFBO();

    //VAO
    void BindVAO(UVertexBuffer *vb);
    void UnbindVAO();

    int CreateVAO();
    void DeleteVAO(UVertexArrayObject *vao);

    //Shaders    
    int  CompileShader(const std::string* source, USHADER_TYPE st, std::vector<std::string> defines);
    void DeleteShader(UShader* shd);

    int CreateShaderProgram(UShader *vertex_sh, UShader *pixel_sh);
    void SetShaderProgram(UShaderProgram *sh);
    void DeleteShaderProgram(UShaderProgram *sh);

    void CacheUniform4(UShaderProgram *sh, std::string name, unsigned int num, float *variable);
    void CacheUniform4(std::string name, unsigned int num, float *variable);

    void CacheUniform1(UShaderProgram *sh, std::string name, unsigned int num, float *variable);
    void CacheUniform1(std::string name, unsigned int num, float *variable);
    void CacheUniform1(std::string name, int value);

    void CacheUniform3(UShaderProgram *sh, std::string name, unsigned int num, float *variable);
    void CacheUniform3(std::string name, unsigned int num, float *variable);

    void CacheUniformMatrix4(std::string name, unsigned int num, float *variable);
    void CacheUniformMatrix3(std::string name, unsigned int num, float *variable);

    void CacheUniformMatrix4(UShaderProgram *sh, std::string name, unsigned int num, float *variable);
    void CacheUniformMatrix3(UShaderProgram *sh, std::string name, unsigned int num, float *variable);

    void Uniform4(unsigned int location, unsigned int num, float *variable);
    void Uniform1(unsigned int location, unsigned int num, float *variable);
    void Uniform1(unsigned int location, int value);
    void Uniform3(unsigned int location, unsigned int num, float *variable);
    void UniformMatrix4(unsigned int location, unsigned int num, float *variable);
    void UniformMatrix3(unsigned int location, unsigned int num, float *variable);

    void DrawSegment(const vec3& p1, const vec3& p2, const vec3& color);
    void DrawTransform(::transform xf);
    void DrawSolidPolygon(const UVertex* vertices, int vertexCount, const vec4 color);

    void DrawBuffer(UVertexBuffer *vb);
    void DrawBuffer(UIndexBuffer* ib);
};