#include "URendererHelper.h"
#include "../UEngine.h"
#include "UFrameBufferObject.h"
#include "../Resources/UTexture.h"

GLenum g_OpenGLError = GL_NO_ERROR;
ILenum g_DevILError = IL_NO_ERROR;

UTexture* URendererHelper::GetTemporaryTexture(int width, int height, UTextureFormat type)
{
    TempTexture text;
    text.width = width;
    text.height = height;
    text.type = type;

    auto res = temporaryTextures[text.ToString()];
    if (res == nullptr)
    {
        res = dynamic_cast<UTexture*>(UEngine::rf.Create(UResourceType::Texture));
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
    text.type = texture->GetFormat();

    UEngine::rf.Release(temporaryTextures[text.ToString()]);
    temporaryTextures.erase(temporaryTextures.find(text.ToString()));
}

void URendererHelper::Initialize()
{
    gauss_blur = dynamic_cast<UPostEffect*>(UEngine::rf.Load("data\\PostEffects\\post_effect_gauss_blur.xml", UResourceType::PostEffect));
    copy_texture = dynamic_cast<UPostEffect*>(UEngine::rf.Load("data\\PostEffects\\post_effect_copy_texture.xml", UResourceType::PostEffect));

    fbo = new UFrameBufferObject();
    fbo->Initialize();
}

void URendererHelper::CopyTexture(UTexture* from_texture, UTexture* to_texture)
{
    auto render = URenderer::GetInstance();

    auto prev_name = from_texture->name;
    from_texture->name = "colorScene";
    to_texture->name = "colorScene";

    copy_texture->ClearUniformUnits();
    copy_texture->AddTexture(from_texture, 0);

    GLenum buffers[] = { (GLenum)UFramebufferAttachment::Color0 };

    fbo->BindTexture(to_texture, UFramebufferAttachment::Color0);
    render->BindFBO(fbo);

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
    glDrawBuffers(1, buffers);
    glViewport(0, 0, to_texture->GetWidth(), to_texture->GetHeight());
    copy_texture->Render(URenderPass::Forward);

    fbo->UnbindTexture(UFramebufferAttachment::Color0);
    render->UnbindFBO();
    
    //to_texture->name = prev_name;
    //from_texture->name = prev_name;

    OPENGL_CHECK_FOR_ERRORS();
}

void URendererHelper::GaussBlur(UTexture* texture, float amount, vec2 dir)
{
    auto render = URenderer::GetInstance();
    auto prev_name = texture->name;

    GLenum buffers[] = { (GLenum)UFramebufferAttachment::Color0 };

    auto temp_texture = GetTemporaryTexture(texture->GetWidth(), texture->GetHeight(), texture->GetFormat());
    CopyTexture(texture, temp_texture);

    gauss_blur->ClearUniformUnits();
    gauss_blur->AddTexture(temp_texture, 0);
    gauss_blur->material.params["resolution"] = UUniformParam((float)texture->GetWidth());
    gauss_blur->material.params["radius"] = UUniformParam(amount);
    gauss_blur->material.params["dir"] = UUniformParam(dir);

    fbo->BindTexture(texture, UFramebufferAttachment::Color0);
    render->BindFBO(fbo);
    glDrawBuffers(1, buffers);
    gauss_blur->Render(URenderPass::Forward);
    render->UnbindFBO();

    //fbo->UnbindTexture(UFramebufferAttachment::Color0);

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