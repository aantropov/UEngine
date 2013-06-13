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
#include <hash_map>
#include <map>

class UVertexBuffer;
class UIndexBuffer;
class UShader;
class UShaderProgram;
class UTexture;
class UScene;
class UFrameBufferObject;

// Renderer
class URenderer: public USingleton<URenderer>{

    //Main window
    UWindow uWnd;

    // Initialization
    bool InitExtensions();

    bool SetVerticalSynchronization(bool enabled);

    // Debug info printing
    void PrintDebugInfo();
    
    // ModelView Matrix Stack
    std::vector<mat4> modelViewMatrixStack;
    UScene* currentScene;

    UShaderProgram *shaderProgram;
    
    // Optimization
    unsigned int previousVAO;
    unsigned int previousIB;
    
    map<unsigned int, hash_map<string, unsigned int>> uniformsCache;
    map<unsigned int, unsigned int> texChannelsCache;

    int CacheUniformLocation(string name);
    int CacheUniformLocation(string name, UShaderProgram *sh);
           
public:
    
    //Scene methods
    void UENGINE_DECLSPEC SetCurrentScene(UScene* currentScene);
    UScene UENGINE_DECLSPEC *GetCurrentScene();
    
    static URenderer UENGINE_DECLSPEC *GetInstance();

    URenderer();
    ~URenderer();

    // It is very important, to contain camera in renderer object
    UCamera camera;
    
    //Creates a window and initializes OpenGL, it uses UConfig
    bool Initialize();

    //Returns HWND of the main window
    HWND GetHWND() { return uWnd.GetHWND(); }

    //Get width and height
    int GetWidth(){ return uWnd.GetWidth(); }
    int GetHeight(){ return uWnd.GetHeight(); }
        
    //Releases a window
    void Release();

    //Operations with Stack
    mat4 modelView;
    void PushModelMatrix();
    void PopModelMatrix();

    //Camera
    UCamera GetCamera();
    void SetCamera(UCamera cam);
    void SetupCameraForShaderProgram(UShaderProgram *shd, mat4 &model);
    void SetupCameraForShaderProgram(UCamera *cam, UShaderProgram *shd, const mat4 &model);
    void SetupCameraLightForShaderProgram(UCamera &camera);

    //Texture
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
    int CompileShader(std::string source, USHADER_TYPE st);
    void DeleteShader(UShader* shd);

    int CreateShaderProgram(UShader *vertex_sh, UShader *pixel_sh);
    void SetShaderProgram(UShaderProgram *sh);
    void DeleteShaderProgram(UShaderProgram *sh);

    void CacheUniform4(UShaderProgram *sh, std::string name, unsigned int num , float *variable);
    void CacheUniform4(std::string name, unsigned int num , float *variable);
    
    void CacheUniform1(UShaderProgram *sh, std::string name, unsigned int num , float *variable);
    void CacheUniform1(std::string name, unsigned int num , float *variable);
    void CacheUniform1(std::string name, int value);

    void CacheUniform3(UShaderProgram *sh, std::string name, unsigned int num , float *variable);
    void CacheUniform3(std::string name, unsigned int num , float *variable);

    void CacheUniformMatrix4(std::string name, unsigned int num , float *variable);
    void CacheUniformMatrix3(std::string name, unsigned int num , float *variable);

    void CacheUniformMatrix4(UShaderProgram *sh, std::string name, unsigned int num , float *variable);
    void CacheUniformMatrix3(UShaderProgram *sh, std::string name, unsigned int num , float *variable);

    void Uniform4(unsigned int location, unsigned int num , float *variable);
    void Uniform1(unsigned int location, unsigned int num , float *variable);
    void Uniform1(unsigned int location, int value);
    void Uniform3(unsigned int location, unsigned int num , float *variable);
    void UniformMatrix4(unsigned int location, unsigned int num , float *variable);
    void UniformMatrix3(unsigned int location, unsigned int num , float *variable);


    //Drawing
    void Quad(vec3 v1,vec3 v2,vec3 v3,vec3 v4 );
    void DrawBuffer(UVertexBuffer *vb);
    void DrawBuffer(UIndexBuffer* ib);

    //GUI
    void InitGui()
    {
	}

    void DrawMyGuiFrame(){ /*mPlatform->getRenderManagerPtr()->drawOneFrame();*/}

};

// Singleton
//URenderer* URenderer::instance = nullptr;