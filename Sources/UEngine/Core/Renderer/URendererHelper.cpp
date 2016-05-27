#include "URendererHelper.h"
#include "../UEngine.h"
#include "UFrameBufferObject.h"
#include "../Resources/UTexture.h"

GLenum g_OpenGLError = GL_NO_ERROR;
ILenum g_DevILError = IL_NO_ERROR;

UTexture* URendererHelper::GetTemporaryTexture(int width, int height, UTEXTURE_TYPE type)
{
    TempTexture text;
    text.width = width;
    text.height = height;
    text.type = type;

    auto res = temporaryTextures[text.ToString()];
    if (res == nullptr)
    {
        res = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
        res->Create(width, height, type);
        temporaryTextures[text.ToString()] = res;
    }

    return res;
}

void URendererHelper::ReleaseTemporaryTexture(UTexture* texture)
{
    TempTexture text;
    text.width = texture->GetWidth();
    text.height = texture->GetHeight();
    text.type = texture->GetType();

    UEngine::rf.Release(temporaryTextures[text.ToString()]);
    temporaryTextures.erase(temporaryTextures.find(text.ToString()));
}

void URendererHelper::Initialize()
{
    gauss_blur = dynamic_cast<UPostEffect*>(UEngine::rf.Load("data\\PostEffects\\post_effect_gauss_blur.xml", URESOURCE_POST_EFFECT));
    copy_texture = dynamic_cast<UPostEffect*>(UEngine::rf.Load("data\\PostEffects\\post_effect_copy_texture.xml", URESOURCE_POST_EFFECT));

    fbo = new UFrameBufferObject();
    fbo->Initialize();
}

void URendererHelper::GaussBlur(UTexture* texture, float amount, vec2 dir)
{
    auto render = URenderer::GetInstance();

    auto temp_texture = GetTemporaryTexture(texture->GetWidth(), texture->GetHeight(), texture->GetType());
    temp_texture->name = "colorScene";

    auto prev_name = texture->name;
    texture->name = "colorScene";

    copy_texture->ClearUniformUnits();
    copy_texture->AddTexture(texture, 0);

    GLenum buffers[] = { UFB_ATTACHMENT_COLOR0 };

    fbo->BindTexture(temp_texture, UFRAMEBUFFER_ATTACHMENT::UFB_ATTACHMENT_COLOR0);
    render->BindFBO(fbo);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
    glDrawBuffers(1, buffers);
    glViewport(0, 0, texture->GetWidth(), texture->GetHeight());
    copy_texture->Render(URENDER_PASS_FORWARD);
    render->UnbindFBO();
    fbo->UnbindTexture(UFRAMEBUFFER_ATTACHMENT::UFB_ATTACHMENT_COLOR0);

    OPENGL_CHECK_FOR_ERRORS();

    gauss_blur->ClearUniformUnits();
    gauss_blur->AddTexture(temp_texture, 0);
    gauss_blur->material.params["resolution"] = UUniformParam(texture->GetWidth());
    gauss_blur->material.params["radius"] = UUniformParam(amount);
    gauss_blur->material.params["dir"] = UUniformParam(dir);

    fbo->BindTexture(texture, UFRAMEBUFFER_ATTACHMENT::UFB_ATTACHMENT_COLOR0);
    render->BindFBO(fbo);
    glDrawBuffers(1, buffers);
    gauss_blur->Render(URENDER_PASS_FORWARD);
    render->UnbindFBO();
    //fbo->UnbindTexture(UFRAMEBUFFER_ATTACHMENT::UFB_ATTACHMENT_COLOR0);

    OPENGL_CHECK_FOR_ERRORS();

    texture->name = prev_name;
}

URendererHelper* URendererHelper::GetInstance()
{
    if (instance == NULL)
    {
        ULogger::GetInstance()->Message("URendererHelper singleton object is created...");
        instance = new URendererHelper();
    }
    return instance;
}
/*
// Extensions OpenGL
// Texture
PFNGLACTIVETEXTUREPROC glActiveTexture = NULL;
// VAO
PFNGLGENVERTEXARRAYSPROC    glGenVertexArrays    = NULL;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = NULL;
PFNGLBINDVERTEXARRAYPROC    glBindVertexArray    = NULL;
// VBO
PFNGLGENBUFFERSPROC    glGenBuffers    = NULL;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = NULL;
PFNGLBINDBUFFERPROC    glBindBuffer    = NULL;
PFNGLBUFFERDATAPROC    glBufferData    = NULL;
PFNGLBUFFERSUBDATAPROC glBufferSubData = NULL;
PFNGLMAPBUFFERPROC     glMapBuffer     = NULL;
PFNGLUNMAPBUFFERPROC   glUnmapBuffer   = NULL;
// Shaders
PFNGLCREATEPROGRAMPROC     glCreateProgram     = NULL;
PFNGLDELETEPROGRAMPROC     glDeleteProgram     = NULL;
PFNGLLINKPROGRAMPROC       glLinkProgram       = NULL;
PFNGLVALIDATEPROGRAMPROC   glValidateProgram   = NULL;
PFNGLUSEPROGRAMPROC        glUseProgram        = NULL;
PFNGLGETPROGRAMIVPROC      glGetProgramiv      = NULL;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;
PFNGLCREATESHADERPROC      glCreateShader      = NULL;
PFNGLDELETESHADERPROC      glDeleteShader      = NULL;
PFNGLSHADERSOURCEPROC      glShaderSource      = NULL;
PFNGLCOMPILESHADERPROC     glCompileShader     = NULL;
PFNGLATTACHSHADERPROC      glAttachShader      = NULL;
PFNGLDETACHSHADERPROC      glDetachShader      = NULL;
PFNGLGETSHADERIVPROC       glGetShaderiv       = NULL;
PFNGLGETSHADERINFOLOGPROC  glGetShaderInfoLog  = NULL;
// Attributes
PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation        = NULL;
PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer      = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray  = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = NULL;
// Uniforms
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
PFNGLUNIFORMMATRIX3FVPROC   glUniformMatrix3fv   = NULL;
PFNGLUNIFORMMATRIX4FVPROC   glUniformMatrix4fv   = NULL;
PFNGLUNIFORM1IPROC          glUniform1i          = NULL;
PFNGLUNIFORM1FVPROC         glUniform1fv         = NULL;
PFNGLUNIFORM3FVPROC         glUniform3fv         = NULL;
PFNGLUNIFORM4FVPROC         glUniform4fv         = NULL;
// FBO
PFNGLBINDFRAMEBUFFERPROC        glBindFramebuffer        = NULL;
PFNGLDELETEFRAMEBUFFERSPROC     glDeleteFramebuffers     = NULL;
PFNGLGENFRAMEBUFFERSPROC        glGenFramebuffers        = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus = NULL;
PFNGLFRAMEBUFFERTEXTUREPROC     glFramebufferTexture     = NULL;
//
PFNGLDRAWBUFFERSPROC glDrawBuffers = NULL;
*/

PFNWGLSWAPINTERVALEXTPROC             wglSwapIntervalEXT = NULL;