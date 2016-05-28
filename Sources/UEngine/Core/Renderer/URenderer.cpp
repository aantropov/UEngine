#include "URenderer.h"
#include "UVertexBuffer.h"
#include "UIndexBuffer.h"
#include "UShader.h"
#include "UShaderProgram.h"
#include "..\Resources\UTexture.h"
#include "..\Basic\UCamera.h"
#include "UFrameBufferObject.h"
#include "URendererHelper.h"
#include "..\math\transform.h"
#include "..\Resources\UCubemap.h"

URenderer::URenderer()
{
    previousVAO = 0;
    previousIB = 0;
    model_view = mat4_identity;
}

URenderer::~URenderer()
{
    ULogger::GetInstance()->Message("URenderer singleton object is deleted...");
}

URenderer* URenderer::GetInstance()
{
    if (instance == NULL)
    {
        ULogger::GetInstance()->Message("URenderer singleton object is created...");
        instance = new URenderer();
    }
    return instance;
}

void URenderer::SetCurrentScene(UScene* current_scene)
{
    this->current_scene = current_scene;
}

UScene* URenderer::GetCurrentScene()
{
    return this->current_scene;
}

void URenderer::SetupCameraLightForShaderProgram(UCamera &camera)
{
    //deprecated
}

void URenderer::SetupCameraForShaderProgram(UShaderProgram *shd, mat4 &model_view)
{
    mat4 view = current_camera.GetView();
    mat4 viewProjection = current_camera.GetProjection() * view;

    UniformMatrix4(shd->locations.transform_model, 1, model_view.m);
    UniformMatrix4(shd->locations.transform_view, 1, view.m);
    UniformMatrix4(shd->locations.transform_viewProjection, 1, viewProjection.m);

    if (shd->locations.transform_viewProjectionInv != -1)
    {
        mat4 viewProjectionInv = inverse(viewProjection);
        UniformMatrix4(shd->locations.transform_viewProjectionInv, 1, viewProjectionInv.m);
    }

    if (shd->locations.transform_normal != -1)
    {
        mat3 normal = transpose(mat3(inverse(model_view)));
        UniformMatrix3(shd->locations.transform_normal, 1, normal.m);
    }

    if (shd->locations.transform_modelViewProjection != -1)
    {
        mat4 modelViewProjection = viewProjection * model_view;
        UniformMatrix4(shd->locations.transform_modelViewProjection, 1, modelViewProjection.m);
    }

    float zFar = current_camera.GetZFar();
    float zNear = current_camera.GetZNear();

    Uniform3(shd->locations.transform_viewPosition, 1, current_camera.GetPosition().v);
    Uniform1(shd->locations.camera_znear, 1, &zNear);
    Uniform1(shd->locations.camera_zfar, 1, &zFar);
    Uniform2(shd->locations.camera_screen_size, 1, vec2(this->GetWidth(), this->GetHeight()).v);
}

void  URenderer::PushModelMatrix()
{
    model_view_matrix_stack.push_back(model_view);
}

void  URenderer::PopModelMatrix()
{
    //Secure code
    if (model_view_matrix_stack.size() > 0)
    {
        model_view = model_view_matrix_stack.back();
        model_view_matrix_stack.pop_back();
    }
    else
        model_view = mat4_identity;
}

void URenderer::SetCurrentCamera(UCamera cam)
{
    current_camera = cam;
}

UCamera URenderer::GetCurrentCamera() const
{
    return current_camera;
}

void URenderer::BindTexture(UTexture *tex)
{
    BindTexture(tex, 0);
}

void URenderer::UnbindTexture(unsigned int channel)
{
    tex_ñhannels_ñache[channel] = -1;
    glActiveTexture(GL_TEXTURE0 + channel);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void URenderer::BindTexture(UTexture *tex, unsigned int channel)
{
    if (tex_ñhannels_ñache[channel] == tex->GetId())
        return;
    else
    {
        tex_ñhannels_ñache[channel] = tex->GetId();
        glActiveTexture(GL_TEXTURE0 + channel);
        glBindTexture(GL_TEXTURE_2D, tex->GetId());
    }
}

int URenderer::CreateCubemap(UCubemap *tex) const
{
    tex->GenTexture();
    GLuint id = tex->GetId();

    auto filter = GL_LINEAR;
    if (tex->GetImageFilter() == UTEXTURE_FILTER_NEAREST)
        filter = GL_NEAREST;

    auto wrap = GL_REPEAT;
    if (tex->GetImageWrap() == UTEXTURE_WRAP_CLAMP)
        wrap = GL_CLAMP_TO_EDGE;

    glEnable(GL_TEXTURE_CUBE_MAP_ARB);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, wrap);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, wrap);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, filter);

    GLenum	intFormat = GL_RGBA8;
    if (tex->GetImageFormat() == IL_RGB || tex->GetImageFormat() == IL_BGR)
        intFormat = GL_RGB8;

    for (int i = 0; i < 6; i++)
    {
        ilBindImage(tex->GetHelperTextures()[i]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + i, 0, intFormat,
            tex->GetWidth(), tex->GetWidth(), 0, tex->GetImageFormat(), tex->GetImageType(), ilGetData());
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, 0);
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
    return id;
}

void URenderer::BindCubemap(UCubemap *tex, unsigned int channel)
{
    if (tex_ñhannels_ñache[channel] == tex->GetId())
        return;
    else
    {
        tex_ñhannels_ñache[channel] = tex->GetId();
        glActiveTexture(GL_TEXTURE0 + channel);
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, tex->GetId());
    }
}

void URenderer::DeleteCubemap(UCubemap *tex) const
{
    GLuint t = tex->GetId();
    OPENGL_CALL(glDeleteTextures(1, &t));
}

int URenderer::CreateTexture(UTexture *tex) const
{
    tex->GenTexture();
    GLuint texture = tex->GetId();

    glBindTexture(GL_TEXTURE_2D, texture);

    auto minFilter = tex->GetMipMap() ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
    auto magFilter = GL_LINEAR;
    if (tex->GetImageFilter() == UTEXTURE_FILTER_NEAREST)
    {
        minFilter = GL_NEAREST;
        magFilter = GL_NEAREST;
    }

    auto wrap = GL_REPEAT;
    if (tex->GetImageWrap() == UTEXTURE_WRAP_CLAMP)
        wrap = GL_CLAMP;
    else if (tex->GetImageWrap() == UTEXTURE_WRAP_CLAMP_TO_EDGE)
        wrap = GL_CLAMP_TO_EDGE;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

    if (tex->GetTextureResId() != 0)
    {
        tex->SetType(UTEXTURE_COLOR);
        OPENGL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, tex->GetImageFormat(), tex->GetWidth(), tex->GetHeight(), 0, tex->GetImageFormat(), tex->GetImageType(), ilGetData()));
    }
    else
    {
        if (tex->GetType() == UTEXTURE_DEPTH_SHADOW)
        {
            //OPENGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_LUMINANCE));
            OPENGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
            //OPENGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL));

            /*OPENGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB,
                GL_COMPARE_R_TO_TEXTURE_ARB));
                OPENGL_CALL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FAIL_VALUE_ARB, 0.5f));
                OPENGL_CALL(glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE));
                */
            OPENGL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, tex->GetWidth(), tex->GetHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
        }
        else if (tex->GetType() == UTEXTURE_RG32F)
        {
            OPENGL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, tex->GetWidth(), tex->GetHeight(), 0, GL_RG, GL_FLOAT, NULL));
        }
        else if (tex->GetType() == UTEXTURE_DEPTH)
        {
            OPENGL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, tex->GetWidth(), tex->GetHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
        }
        else if (tex->GetType() == UTEXTURE_COLOR)
        {
            OPENGL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->GetWidth(), tex->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
        }
        else if (tex->GetType() == UTEXTURE_FLOAT)
        {
            OPENGL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->GetWidth(), tex->GetHeight(), 0, GL_RGBA, GL_FLOAT, NULL));
        }
        else if (tex->GetType() == UTEXTURE_FLOAT32)
        {
            OPENGL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, tex->GetWidth(), tex->GetHeight(), 0, GL_RED, GL_FLOAT, NULL));
        }
    }

    if (tex->GetMipMap())
    {
        glGenerateMipmap(GL_TEXTURE_2D);
        glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
    }

    OPENGL_CHECK_FOR_ERRORS();
    return texture;
}

void URenderer::DeleteTexture(UTexture *tex) const
{
    GLuint t = tex->GetId();
    OPENGL_CALL(glDeleteTextures(1, &t));
}

void URenderer::UnbindFBO() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void URenderer::BindFBO(UFrameBufferObject *fb) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, fb->GetId());
}

int URenderer::CreateFBO() const
{
    GLuint depthFBO = 0;

    glGenFramebuffers(1, &depthFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

    return depthFBO;
}

void URenderer::DeleteFBO(UFrameBufferObject *fb) const
{
    GLuint fbo = fb->GetId();
    OPENGL_CALL(glDeleteBuffers(1, &fbo));
}

int URenderer::CreateVBO(UVertexBuffer *vb, UVBO_DRAW state) const
{
    int size = vb->GetNum()*sizeof(UVertex);

    GLuint vbo;
    OPENGL_CALL(glGenBuffers(1, &vbo));
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    OPENGL_CALL(glBufferData(GL_ARRAY_BUFFER, size, vb->GetPointer(), GL_STREAM_DRAW));

    return vbo;
}

int URenderer::CreateVBO(UIndexBuffer *ib, UVBO_DRAW state) const
{
    int size = ib->GetNum()*sizeof(unsigned int);

    GLuint vbo;
    OPENGL_CALL(glGenBuffers(1, &vbo));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
    OPENGL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, ib->GetPointer(), state));

    return vbo;
}

void URenderer::DeleteVBO(UBuffer *vb)  const
{
    GLuint vbo = vb->GetId();
    OPENGL_CALL(glDeleteBuffers(1, &vbo));
}

void URenderer::DrawSegment(const vec3& p1, const vec3& p2, const vec3& color)
{
#ifdef UE_DEBUG

    glColor4f(color.x, color.y, color.z, 1);
    GLfloat glVertices[] =
    {
        p1.x, p1.y, p1.z, p2.x, p2.y, p2.z
    };

    glVertexPointer(3, GL_FLOAT, 0, glVertices);
    glDrawArrays(GL_LINES, 0, 2);
#endif
}

void URenderer::DrawTransform(::transform xf)
{
#ifdef UE_DEBUG
    vec3 p1 = xf.position;
    vec3 p2;

    const float k_axisScale = 0.4f;

    p2 = p1 + (xf.matrix() * vec4_x) * k_axisScale;
    DrawSegment(p1, p2, vec3(1, 0, 0));

    p2 = p1 + (xf.matrix() * vec4_y) * k_axisScale;
    DrawSegment(p1, p2, vec3(0, 1, 0));

    p2 = p1 + (xf.matrix() * vec4_z) * k_axisScale;
    DrawSegment(p1, p2, vec3(0, 0, 1));
#endif
}

void URenderer::DrawSolidPolygon(const UVertex* vertices, int vertexCount, const vec4 color)
{
#ifdef UE_DEBUG
    GLfloat glverts[24];
    glVertexPointer(3, GL_FLOAT, 0, glverts);
    glEnableClientState(GL_VERTEX_ARRAY);

    for (int i = 0; i < vertexCount; i++)
    {
        glverts[i * 3] = vertices[i].GetPosition().x;
        glverts[i * 3 + 1] = vertices[i].GetPosition().y;
        glverts[i * 3 + 2] = vertices[i].GetPosition().z;
    }

    glColor4f(color.x, color.y, color.z, 1);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);

    glLineWidth(3);
    glColor4f(1, 0, 1, 1);
    glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
#endif
}

void URenderer::DrawBuffer(UVertexBuffer* vb)
{
    draw_ñalls++;
    OPENGL_CALL(glDrawArrays(GL_TRIANGLES, 0, vb->GetNum()));
}

void URenderer::DrawBuffer(UIndexBuffer* ib)
{
    draw_ñalls++;
    OPENGL_CALL(glDrawElements(GL_TRIANGLES, ib->GetNum(), GL_UNSIGNED_INT, NULL));
}

void URenderer::BindVBO(UVertexBuffer *vb) const
{
    glBindBuffer(GL_ARRAY_BUFFER, vb->GetId());
}

void URenderer::BindVBO(UIndexBuffer *vb)
{
    if (previousIB != vb->GetId())
    {
        previousIB = vb->GetId();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vb->GetId());
    }
}

void URenderer::UnbindVBO(bool vertex_buffer) const
{
    if (vertex_buffer)
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    else
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//VAO
int URenderer::CreateVAO() const
{
    GLuint vao;
    OPENGL_CALL(glGenVertexArrays(1, &vao));
    glBindVertexArray(vao);
    return vao;
}

void URenderer::DrawDebugLine(vec3 start, vec3 end, vec3 color)
{
#ifdef UE_DEBUG
    mat4 view = current_camera.GetView();
    mat4 viewProjection = current_camera.GetProjection() * view;
    mat4 viewProjectionInv = inverse(viewProjection);

    start = transpose(viewProjectionInv) * start;
    end = transpose(viewProjectionInv) * end;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    glLineWidth(3);
    glDisable(GL_TEXTURE_2D);
    glColor3f(color.x, color.y, color.z);
    glBegin(GL_LINES);
    glVertex3f(start.x, start.y, start.z);
    glVertex3f(end.x, end.y, end.z);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
#endif
}

void URenderer::DeleteVAO(UVertexArrayObject *vao) const
{
    GLuint id = vao->GetId();
    OPENGL_CALL(glDeleteVertexArrays(1, &id));
}

void URenderer::BindVAO(UVertexBuffer *vb)
{
    if (previousVAO != vb->GetVAO()->GetId())
    {
        previousVAO = vb->GetVAO()->GetId();
        glBindVertexArray(vb->GetVAO()->GetId());
    }
}

void URenderer::UnbindVAO() const
{
    glBindVertexArray(0);
}

//Shaders
int  URenderer::CompileShader(const std::string* source, USHADER_TYPE st, std::vector<std::string> defines) const
{
    GLuint shd;
    GLchar *strings = (GLchar*)source->c_str();

    std::string shader_define;
    std::string define_vertex(GetShaderVersion() + "\n#define VERTEX\n");
    std::string define_fragment(GetShaderVersion() + "\n#define FRAGMENT\n");

    if (st == USHADER_VERTEX)
        shader_define = define_vertex;
    else if (st == USHADER_FRAGMENT)
        shader_define = define_fragment;

    std::vector<GLchar*> shader_strings(defines.size() + 2);
    std::vector<GLint> shader_strings_lengths(defines.size() + 2);

    shader_strings[0] = (GLchar*)shader_define.c_str();
    shader_strings_lengths[0] = (GLint)strlen(shader_define.c_str());

    for (int i = 0; i < defines.size(); i++)
    {
        defines[i] = "#define " + defines[i] + "\n";
        shader_strings[i + 1] = (GLchar*)defines[i].c_str();
        shader_strings_lengths[i + 1] = (GLint)strlen(defines[i].c_str());
    }

    shader_strings[defines.size() + 1] = strings;
    shader_strings_lengths[defines.size() + 1] = (GLint)strlen((char*)strings);

    shd = glCreateShader(st);
    glShaderSource(shd, defines.size() + 2, (const GLchar**)&shader_strings[0], (const GLint*)&shader_strings_lengths[0]);
    glCompileShader(shd);

    OPENGL_CHECK_FOR_ERRORS();

    GLint status;
    glGetShaderiv(shd, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        GLint length;
        glGetShaderiv(shd, GL_INFO_LOG_LENGTH, &length);

        std::string log(length, '\0');
        glGetShaderInfoLog(shd, length, &length, &log[0]);

        ULogger::GetInstance()->Message(log, ULOG_MSG_ERROR, ULOG_OUT_FILE);
    }

    return shd;
}

void URenderer::DeleteShader(UShader *shd) const
{
    OPENGL_CALL(glDeleteShader(shd->GetId()));
}

int URenderer::CreateShaderProgram(UShader *vertex_sh, UShader *pixel_sh) const
{
    GLuint sh_pr_id = glCreateProgram();
    OPENGL_CALL(glAttachShader(sh_pr_id, vertex_sh->GetId()));
    OPENGL_CALL(glAttachShader(sh_pr_id, pixel_sh->GetId()));
    OPENGL_CALL(glLinkProgram(sh_pr_id));

    OPENGL_CHECK_FOR_ERRORS();

    GLint status;
    glGetProgramiv(sh_pr_id, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        GLint length = 0;
        glGetProgramiv(sh_pr_id, GL_INFO_LOG_LENGTH, &length);

        std::string log(length, '\0');
        glGetProgramInfoLog(sh_pr_id, length, &length, &log[0]);

        ULogger::GetInstance()->Message(log, ULOG_MSG_ERROR, ULOG_OUT_FILE);
    }

    return sh_pr_id;
}

void URenderer::DeleteShaderProgram(UShaderProgram *shd)
{
    CacheReleaseUniformLocation(shd);
    OPENGL_CALL(glDeleteProgram(shd->GetId()));
}

void URenderer::SetShaderProgram(UShaderProgram *sh)
{
    if (sh == nullptr || (shader_program != nullptr && sh == shader_program))
        return;

    shader_program = sh;
    OPENGL_CALL(glUseProgram(sh->GetId()));
    OPENGL_CHECK_FOR_ERRORS();
}

void  URenderer::CacheReleaseUniformLocation(UShaderProgram* sh)
{
    uniforms_ñache.erase(sh->GetId());
}

int URenderer::CacheUniformLocation(string name)
{
    return CacheUniformLocation(name, shader_program);
}

int URenderer::CacheUniformLocation(string name, UShaderProgram *sh)
{
    unsigned int *res = &uniforms_ñache[sh->GetId()][name];
    if (*res == 0)
    {
        unsigned int  loc = glGetUniformLocation(sh->GetId(), name.c_str());

        if (loc > MAX_UNIFORM_LOCATIONS)
            loc = MAX_UNIFORM_LOCATIONS + 1;
        *res = loc;
    }
    return *res;
}

void URenderer::CacheUniform4(UShaderProgram *sh, std::string name, unsigned int num, float *variable)
{
    Uniform4(CacheUniformLocation(name, sh), num, variable);
}

void URenderer::CacheUniform4(std::string name, unsigned int num, float *variable)
{
    Uniform4(CacheUniformLocation(name), num, variable);
}

void URenderer::CacheUniformMatrix4(std::string name, unsigned int num, float *variable)
{
    UniformMatrix4(CacheUniformLocation(name), num, variable);
}

void URenderer::CacheUniformMatrix3(UShaderProgram *sh, std::string name, unsigned int num, float *variable)
{
    UniformMatrix3(CacheUniformLocation(name, sh), num, variable);
}

void URenderer::CacheUniformMatrix4(UShaderProgram *sh, std::string name, unsigned int num, float *variable)
{
    UniformMatrix4(CacheUniformLocation(name, sh), num, variable);
}

void URenderer::CacheUniformMatrix3(std::string name, unsigned int num, float *variable)
{
    UniformMatrix3(CacheUniformLocation(name), num, variable);
}

void URenderer::CacheUniform1(UShaderProgram *sh, std::string name, unsigned int num, float *variable)
{
    Uniform1(CacheUniformLocation(name, sh), num, variable);
}

void URenderer::CacheUniform1(std::string name, unsigned int num, float *variable)
{
    Uniform1(CacheUniformLocation(name), num, variable);
}

void URenderer::CacheUniform1(std::string name, int value)
{
    Uniform1(CacheUniformLocation(name), value);
}

void URenderer::CacheUniform3(UShaderProgram *sh, std::string name, unsigned int num, float *variable)
{
    Uniform3(CacheUniformLocation(name, sh), num, variable);
}

void URenderer::CacheUniform3(std::string name, unsigned int num, float *variable)
{
    Uniform3(CacheUniformLocation(name), num, variable);
}

void URenderer::CacheUniform2(std::string name, unsigned int num, float *variable)
{
    Uniform2(CacheUniformLocation(name), num, variable);
}

void URenderer::CacheUniform2(UShaderProgram *sh, std::string name, unsigned int num, float *variable)
{
    Uniform2(CacheUniformLocation(name, sh), num, variable);
}
///////////////

void URenderer::Uniform4(const unsigned int location, const unsigned int num, const float *variable) const
{
    if (location < MAX_UNIFORM_LOCATIONS)
        glUniform4fv(location, num, variable);
}

void URenderer::UniformMatrix4(const unsigned int location, const unsigned int num, const float *variable) const
{
    if (location < MAX_UNIFORM_LOCATIONS)
        glUniformMatrix4fv(location, num, GL_TRUE, variable);
}

void URenderer::UniformMatrix3(const unsigned int location, const unsigned int num, const  float *variable) const
{
    if (location < MAX_UNIFORM_LOCATIONS)
        glUniformMatrix3fv(location, num, GL_TRUE, variable);
}

void URenderer::Uniform1(const unsigned int location, const unsigned int num, const float *variable) const
{
    if (location < MAX_UNIFORM_LOCATIONS)
        glUniform1fv(location, num, variable);
}

void URenderer::Uniform1(const unsigned int location, const int value) const
{
    if (location < MAX_UNIFORM_LOCATIONS)
        glUniform1i(location, value);
}

void URenderer::Uniform1(const unsigned int location, const unsigned int num, const int* value) const
{
    if (location < MAX_UNIFORM_LOCATIONS)
        glUniform1iv(location, num, value);
}

void URenderer::Uniform3(const unsigned int location, const unsigned int num, const float *variable) const
{
    if (location < MAX_UNIFORM_LOCATIONS)
        glUniform3fv(location, num, variable);
}

void URenderer::Uniform2(const unsigned int location, const unsigned int num, const float *variable) const
{
    if (location < MAX_UNIFORM_LOCATIONS)
        glUniform2fv(location, num, variable);
}

bool URenderer::SetVerticalSynchronization(bool bEnabled)
{
    if (!wglSwapIntervalEXT)
        return false;

    if (bEnabled)
        wglSwapIntervalEXT(1);
    else
        wglSwapIntervalEXT(0);

    return true;
}

bool URenderer::Initialize()
{
    auto config = UConfig::GetInstance();

    if (!window.Create(L"UEngine", config->GetParami("/xml/config/width/"), config->GetParami("/xml/config/height/"), config->GetParami("/xml/config/fullscreen/") == 1))
        return false;

    if (!InitExtensions())
        return false;

    OPENGL_CALL(glActiveTexture(GL_TEXTURE0));

    OPENGL_CALL(glClearColor(0.2f, 0.2f, 0.2f, 1.0f));
    OPENGL_CALL(glClearDepth(1.0f));

    //OPENGL_CALL(glDepthFunc(GL_LEQUAL));
    //OPENGL_CALL(glDepthRange(0.0f, 1.0f));
    //OPENGL_CALL(glEnable(GL_DEPTH_TEST));
    OPENGL_CALL(glEnable(GL_CULL_FACE));
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    SetVerticalSynchronization(config->GetParami("/xml/config/vsync/") == 1);

    OPENGL_CHECK_FOR_ERRORS();

    //Initialize camera
    float aspectRatio = config->GetParamf("/xml/config/width/") / config->GetParamf("/xml/config/height/");
    main_ñamera.Create(0.0f, 1.0f, 0.0f);
    main_ñamera.Perspective(60.0f, aspectRatio, 0.01f, 1000.0f);
    //main_ñamera.Ortho(-100, 100, -100, 100, 0.1, 4000);
    //main_ñamera.SetPosition(vec3_zero);
    main_ñamera.SetRotation(vec3_y * 90.0f);

    shadow_bias = config->GetParamf("/xml/config/shadow_bias/");

    //init DevIl
    DEVIL_CALL(ilInit());
    DEVIL_CALL(iluInit());
    DEVIL_CALL(ilutInit());

    DEVIL_CALL(ilutRenderer(ILUT_OPENGL));
    DEVIL_CALL(ilSetInteger(IL_KEEP_DXTC_DATA, IL_TRUE));
    DEVIL_CALL(ilutEnable(ILUT_GL_AUTODETECT_TEXTURE_TARGET));
    DEVIL_CALL(ilutEnable(ILUT_OPENGL_CONV));
    DEVIL_CALL(ilutEnable(ILUT_GL_USE_S3TC));

    if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
        ULogger::GetInstance()->Message("Incorrect devil.dll version");

    if (iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION)
        ULogger::GetInstance()->Message("Incorrect ilu.dll version");

    if (ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION)
        ULogger::GetInstance()->Message("Incorrect ilut.dll version");

    DEVIL_CHECK_FOR_ERRORS();

    PrintDebugInfo();

    URendererHelper::GetInstance()->Initialize();
    return true;
}

void URenderer::Release()
{
    window.Destroy();
}

void URenderer::PrintDebugInfo()
{
    GLint major, minor, mrt;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &mrt);

    char message[1024];

    sprintf_s(message, "\nOpenGL render context information:\n"
        "  Renderer       : %s\n"
        "  Vendor         : %s\n"
        "  Version        : %s\n"
        "  GLSL version   : %s\n"
        "  OpenGL version : %d.%d\n"
        "  Max Render Targets: %d\n"
        "  DevIL version  : %d\n",
        (const char*)glGetString(GL_RENDERER),
        (const char*)glGetString(GL_VENDOR),
        (const char*)glGetString(GL_VERSION),
        (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION),
        major, minor, mrt,
        (int)ilGetInteger(IL_VERSION_NUM)
        );

    ULogger::GetInstance()->Message(message);

    OPENGL_CHECK_FOR_ERRORS();
    DEVIL_CHECK_FOR_ERRORS();
}

bool URenderer::InitExtensions()
{
    /*
    // Texture
    OPENGL_GET_PROC(PFNGLACTIVETEXTUREPROC, glActiveTexture);
    // VAO
    OPENGL_GET_PROC(PFNGLGENVERTEXARRAYSPROC,    glGenVertexArrays);
    OPENGL_GET_PROC(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays);
    OPENGL_GET_PROC(PFNGLBINDVERTEXARRAYPROC,    glBindVertexArray);
    // VBO
    OPENGL_GET_PROC(PFNGLGENBUFFERSPROC,    glGenBuffers);
    OPENGL_GET_PROC(PFNGLDELETEBUFFERSPROC, glDeleteBuffers);
    OPENGL_GET_PROC(PFNGLBINDBUFFERPROC,    glBindBuffer);
    OPENGL_GET_PROC(PFNGLBUFFERDATAPROC,    glBufferData);
    OPENGL_GET_PROC(PFNGLBUFFERSUBDATAPROC, glBufferSubData);
    OPENGL_GET_PROC(PFNGLMAPBUFFERPROC,     glMapBuffer);
    OPENGL_GET_PROC(PFNGLUNMAPBUFFERPROC,   glUnmapBuffer);
    // Shaders
    OPENGL_GET_PROC(PFNGLCREATEPROGRAMPROC,     glCreateProgram);
    OPENGL_GET_PROC(PFNGLDELETEPROGRAMPROC,     glDeleteProgram);
    OPENGL_GET_PROC(PFNGLLINKPROGRAMPROC,       glLinkProgram);
    OPENGL_GET_PROC(PFNGLVALIDATEPROGRAMPROC,   glValidateProgram);
    OPENGL_GET_PROC(PFNGLUSEPROGRAMPROC,        glUseProgram);
    OPENGL_GET_PROC(PFNGLGETPROGRAMIVPROC,      glGetProgramiv);
    OPENGL_GET_PROC(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);
    OPENGL_GET_PROC(PFNGLCREATESHADERPROC,      glCreateShader);
    OPENGL_GET_PROC(PFNGLDELETESHADERPROC,      glDeleteShader);
    OPENGL_GET_PROC(PFNGLSHADERSOURCEPROC,      glShaderSource);
    OPENGL_GET_PROC(PFNGLCOMPILESHADERPROC,     glCompileShader);
    OPENGL_GET_PROC(PFNGLATTACHSHADERPROC,      glAttachShader);
    OPENGL_GET_PROC(PFNGLDETACHSHADERPROC,      glDetachShader);
    OPENGL_GET_PROC(PFNGLGETSHADERIVPROC,       glGetShaderiv);
    OPENGL_GET_PROC(PFNGLGETSHADERINFOLOGPROC,  glGetShaderInfoLog);
    // Attributes
    OPENGL_GET_PROC(PFNGLGETATTRIBLOCATIONPROC,        glGetAttribLocation);
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBPOINTERPROC,      glVertexAttribPointer);
    OPENGL_GET_PROC(PFNGLENABLEVERTEXATTRIBARRAYPROC,  glEnableVertexAttribArray);
    OPENGL_GET_PROC(PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray);
    // Uniforms
    OPENGL_GET_PROC(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
    OPENGL_GET_PROC(PFNGLUNIFORMMATRIX3FVPROC,   glUniformMatrix3fv);
    OPENGL_GET_PROC(PFNGLUNIFORMMATRIX4FVPROC,   glUniformMatrix4fv);
    OPENGL_GET_PROC(PFNGLUNIFORM1IPROC,          glUniform1i);
    OPENGL_GET_PROC(PFNGLUNIFORM1FVPROC,         glUniform1fv);
    OPENGL_GET_PROC(PFNGLUNIFORM3FVPROC,         glUniform3fv);
    OPENGL_GET_PROC(PFNGLUNIFORM4FVPROC,         glUniform4fv);
    // FBO
    OPENGL_GET_PROC(PFNGLBINDFRAMEBUFFERPROC,        glBindFramebuffer);
    OPENGL_GET_PROC(PFNGLDELETEFRAMEBUFFERSPROC,     glDeleteFramebuffers);
    OPENGL_GET_PROC(PFNGLGENFRAMEBUFFERSPROC,        glGenFramebuffers);
    OPENGL_GET_PROC(PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus);
    OPENGL_GET_PROC(PFNGLFRAMEBUFFERTEXTUREPROC,     glFramebufferTexture);
    //
    OPENGL_GET_PROC(PFNGLDRAWBUFFERSPROC,             glDrawBuffers);

    OPENGL_GET_PROC(PFNWGLSWAPINTERVALEXTPROC,         wglSwapIntervalEXT);
    */
    OPENGL_CHECK_FOR_ERRORS();
    return true;
}
