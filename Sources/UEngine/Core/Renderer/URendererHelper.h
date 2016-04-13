#pragma once

#include "windows.h"
#include "..\Utils\ULogger.h"
#include "..\Utils\enum.h"

#define GL_GLEXT_PROTOTYPES
#define GL3_PROTOTYPES 1

#include <GL\glew.h>
#include <GL\wglew.h>

//#include "mygui 3.2.0/GL/glew.h"
//#include <GL\gl.h>

#include "GL\glext.h"
#include "GL\wglext.h"

#include "IL\il.h"
#include "IL\ilut.h"
#include "IL\ilu.h"

#pragma comment(lib, "DevIL.lib")
#pragma comment(lib, "Ilut.lib")
#pragma comment(lib, "Ilu.lib")

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glew32.lib")

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <unordered_map>
#include <map>
#include "../math/vec2.h"

// Global variable for error reporting
extern GLenum g_OpenGLError;
extern ILenum g_DevILError;

// Safe OpenGL functions calling
#ifdef UE_DEBUG

// Get function address from the driver
#define OPENGL_GET_PROC(p,n) \
    n = (p)wglGetProcAddress(#n); \
    if (n == NULL) \
    { \
    ULogger::GetInstance()->Message("Loading extension \"" #n "\" is failed", ULOG_MSG_ERROR); \
        /*return false;*/ \
    }

#define DEVIL_CHECK_FOR_ERRORS() \
    if ((g_DevILError = ilGetError()) != IL_NO_ERROR) \
    { \
        char message[UE_MAXCHAR]; \
        sprintf_s(message, "DevIL error 0x%X", (unsigned)g_DevILError); \
        ULogger::GetInstance()->Message(message, ULOG_MSG_ERROR, ULOG_OUT_MSG); \
    }

#define OPENGL_CHECK_FOR_ERRORS() \
    if ((g_OpenGLError = glGetError()) != GL_NO_ERROR) \
    { \
        char message[UE_MAXCHAR]; \
        sprintf_s(message, "OpenGL error 0x%X", (unsigned)g_OpenGLError); \
        ULogger::GetInstance()->Message(message, ULOG_MSG_ERROR, ULOG_OUT_MSG); \
    }

#define OPENGL_CALL(expression) \
    { \
        expression; \
        if ((g_OpenGLError = glGetError()) != GL_NO_ERROR) \
        { \
            char message[UE_MAXCHAR]; \
            sprintf_s(message,"OpenGL expression \"" #expression "\" error %d\n", (int)g_OpenGLError); \
            ULogger::GetInstance()->Message(message, ULOG_MSG_ERROR, ULOG_OUT_MSG); \
        } \
    }

#define DEVIL_CALL(expression) \
    { \
        expression; \
        if ((g_DevILError = ilGetError()) != IL_NO_ERROR) \
        { \
            char message[UE_MAXCHAR]; \
            sprintf_s(message,"DevIL expression \"" #expression "\" error %d\n", (int)g_DevILError); \
            ULogger::GetInstance()->Message(message, ULOG_MSG_ERROR, ULOG_OUT_MSG); \
        } \
    }
#endif //UE_DEBUG

#ifdef UE_RELEASE
#define DEVIL_CHECK_FOR_ERRORS() ;
#define OPENGL_CHECK_FOR_ERRORS() ;
//#undef OPENGL_GET_PROC(p,n)
//#undef OPENGL_CALL(expression)

#define OPENGL_CALL(expression) expression;
#define OPENGL_GET_PROC(p,n) \
    n = (p)wglGetProcAddress(#n); \
    if (n == NULL) \
    { \
    ULogger::GetInstance()->Message("Loading extension \"" #n "\" is failed", ULOG_MSG_ERROR); \
        return false; \
    }

#define DEVIL_CALL(expression) expression;
#endif //UE_RELEASE


class UTexture;
class UPostEffect;
class UFrameBufferObject;

class URendererHelper : USingleton<URendererHelper>
{
    class TempTexture
    {
    public:
        int width;
        int height;
        UTEXTURE_TYPE type;              

        string ToString()
        {
            char buffer[64];
            memset(buffer, 256, sizeof(char));

            sprintf_s(buffer, "%d%d%d", width, height, type);
            
            return string(buffer);
        }
    };

    UPostEffect* gauss_blur;
    UPostEffect* copy_texture;
    UFrameBufferObject* fbo;
    
    std::map<string, UTexture*> temporaryTextures;

public:

    static URendererHelper *GetInstance();

    void Initialize();
    void GaussBlur(UTexture* texture, float amount, vec2 dir);

    UTexture* GetTemporaryTexture(int width, int height, UTEXTURE_TYPE type);
    void ReleaseTemporaryTexture(UTexture* texture);
};

/*
// extensions OpenGL
// Texture
extern PFNGLACTIVETEXTUREPROC glActiveTexture;
// VAO
extern PFNGLGENVERTEXARRAYSPROC    glGenVertexArrays;
extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC    glBindVertexArray;
// VBO
extern PFNGLGENBUFFERSPROC    glGenBuffers;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLBINDBUFFERPROC    glBindBuffer;
extern PFNGLBUFFERDATAPROC    glBufferData;
extern PFNGLBUFFERSUBDATAPROC glBufferSubData;
extern PFNGLMAPBUFFERPROC     glMapBuffer;
extern PFNGLUNMAPBUFFERPROC   glUnmapBuffer;
// Shaders
extern PFNGLCREATEPROGRAMPROC     glCreateProgram;
extern PFNGLDELETEPROGRAMPROC     glDeleteProgram;
extern PFNGLLINKPROGRAMPROC       glLinkProgram;
extern PFNGLVALIDATEPROGRAMPROC   glValidateProgram;
extern PFNGLUSEPROGRAMPROC        glUseProgram;
extern PFNGLGETPROGRAMIVPROC      glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLCREATESHADERPROC      glCreateShader;
extern PFNGLDELETESHADERPROC      glDeleteShader;
extern PFNGLSHADERSOURCEPROC      glShaderSource;
extern PFNGLCOMPILESHADERPROC     glCompileShader;
extern PFNGLATTACHSHADERPROC      glAttachShader;
extern PFNGLDETACHSHADERPROC      glDetachShader;
extern PFNGLGETSHADERIVPROC       glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC  glGetShaderInfoLog;
// Attributes
extern PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation;
extern PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
// Uniforms
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLUNIFORMMATRIX3FVPROC   glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC   glUniformMatrix4fv;
extern PFNGLUNIFORM1IPROC          glUniform1i;
extern PFNGLUNIFORM1FVPROC         glUniform1fv;
extern PFNGLUNIFORM3FVPROC         glUniform3fv;
extern PFNGLUNIFORM4FVPROC         glUniform4fv;
// FBO
extern PFNGLBINDFRAMEBUFFERPROC        glBindFramebuffer;
extern PFNGLDELETEFRAMEBUFFERSPROC     glDeleteFramebuffers;
extern PFNGLGENFRAMEBUFFERSPROC        glGenFramebuffers;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
extern PFNGLFRAMEBUFFERTEXTUREPROC     glFramebufferTexture;
//
extern PFNGLDRAWBUFFERSPROC               glDrawBuffers;
*/

//extern PFNWGLSWAPINTERVALEXTPROC       wglSwapIntervalEXT;
