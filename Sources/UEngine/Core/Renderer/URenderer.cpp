#include "URenderer.h"
#include "UVertexBuffer.h"
#include "UIndexBuffer.h"
#include "UShader.h"
#include "UShaderProgram.h"
#include "..\Resources\UTexture.h"
#include "..\Basic\UCamera.h"
#include "UFrameBufferObject.h"

URenderer::URenderer(){

	previousVAO = 0;
	previousIB = 0;
	modelView = mat4_identity;
}
URenderer::~URenderer(){
	ULogger::GetInstance()->Message("URenderer singleton object is deleted...");
}

URenderer* URenderer::GetInstance(){

	if(instance == NULL){
		ULogger::GetInstance()->Message("URenderer singleton object is created...");
		instance = new URenderer();		
	}
	return instance;
}
void URenderer:: SetCurrentScene(UScene* currentScene)
{
	this->currentScene = currentScene;
}

UScene* URenderer:: GetCurrentScene(){
	return this->currentScene;
}
void URenderer:: SetupCameraLightForShaderProgram(UCamera &camera){
   //deprecated
}

void URenderer:: SetupCameraForShaderProgram(UShaderProgram *shd, mat4 &model){

	mat4 view           = camera.GetView();
	mat4 viewProjection = camera.GetProjection() * view;
	mat3 normal         = transpose(mat3(inverse(model)));
	mat4 modelViewProjection = model * viewProjection;
		
	UniformMatrix4(shd->locations.transform_model,  1, model.m);
	UniformMatrix4(shd->locations.transform_viewProjection, 1, viewProjection.m);
	UniformMatrix3(shd->locations.transform_normal, 1, normal.m);
	UniformMatrix4(shd->locations.transform_modelViewProjection, 1, modelViewProjection.m);
	Uniform3(shd->locations.transform_viewPosition, 1, camera.GetPosition().v);
	
}
void  URenderer::PushModelMatrix(){
	modelViewMatrixStack.push_back(modelView);
	//modelView = mat4_identity;
}
void  URenderer::PopModelMatrix(){	
	//Secure code
	if(modelViewMatrixStack.size() > 0){
		modelView = modelViewMatrixStack.back() ;
		modelViewMatrixStack.pop_back();
	}
	else
		modelView = mat4_identity;
}
void URenderer:: SetCamera(UCamera cam){
	camera = cam;
}
UCamera URenderer:: GetCamera(){
	return camera;
}
void URenderer:: Quad(vec3 v1,vec3 v2,vec3 v3,vec3 v4 ){
	//OPENGL_CALL should NOT be used in the glBegin - glEnd block!
	glBegin(GL_TRIANGLE_STRIP);
		glVertex3d(v1.x,v1.y,v1.z);
		glTexCoord2d(0,0);
		glVertex3d(v2.x,v2.y,v2.z);
		glTexCoord2d(0,1);
		glVertex3d(v4.x,v4.y,v4.z);
		glTexCoord2d(1,1);
		glVertex3d(v3.x,v3.y,v3.z);
		glTexCoord2d(1,0);		
	glEnd();
}
//Texture
void  URenderer:: BindTexture(UTexture *tex)
{
	BindTexture(tex, 0);
}
void  URenderer:: BindTexture(UTexture *tex, unsigned int channel)
{
	if(texChannelsCache[channel] == tex->GetId())
		return;
	else
	{
		texChannelsCache[channel] = tex->GetId();
		glActiveTexture(GL_TEXTURE0 + channel);
		glBindTexture(GL_TEXTURE_2D, tex->GetId());
	}
}
int URenderer:: CreateTexture(UTexture *tex){
	
	tex->GenTexture();
	GLuint texture = tex->GetId();
	
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if(tex->GetTextureResId() != 0)
	{
		tex->SetType(UTEXTURE_COLOR);
		OPENGL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger ( IL_IMAGE_FORMAT ) , tex->GetWidth(), tex->GetHeight(), 0, ilGetInteger ( IL_IMAGE_FORMAT ) , ilGetInteger ( IL_IMAGE_TYPE    ), ilGetData()));
	}
	else
	{
		if(tex->GetType() == UTEXTURE_DEPTH_SHADOW){
			OPENGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
			OPENGL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, tex->GetWidth(), tex->GetHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
		}
		else if(tex->GetType() == UTEXTURE_DEPTH){
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
			OPENGL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, tex->GetWidth(), tex->GetHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
		}
		else if(tex->GetType() == UTEXTURE_COLOR){
			OPENGL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->GetWidth(), tex->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
		}
		else if(tex->GetType() == UTEXTURE_FLOAT){
			OPENGL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->GetWidth(), tex->GetHeight(), 0, GL_RGBA, GL_FLOAT, NULL));
		}
	}

	OPENGL_CHECK_FOR_ERRORS();
	return texture;
}

void URenderer:: DeleteTexture(UTexture *tex){
	GLuint t = tex->GetId();
	OPENGL_CALL(glDeleteTextures(1, &t));
}

// FBO
void URenderer:: UnbindFBO(){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void URenderer:: BindFBO(UFrameBufferObject *fb){
	glBindFramebuffer(GL_FRAMEBUFFER, fb->GetId());
}
int URenderer:: CreateFBO(){
	
	GLuint depthFBO = 0;
	
	glGenFramebuffers(1, &depthFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

//	glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);
	
	return depthFBO;
}
void URenderer:: DeleteFBO(UFrameBufferObject *fb){
	GLuint fbo =  fb->GetId();
	OPENGL_CALL(glDeleteBuffers(1, &fbo));
}
// VBO
int URenderer:: CreateVBO(UVertexBuffer *vb, UVBO_DRAW state){
	int size = vb->GetNum()*sizeof(UVertex);
		
	GLuint vbo;
	OPENGL_CALL(glGenBuffers ( 1, &vbo ));
    glBindBuffer ( GL_ARRAY_BUFFER , vbo );
	OPENGL_CALL(glBufferData ( GL_ARRAY_BUFFER , size, vb->GetPointer(), state ));		 
	
	return vbo;
}
int URenderer:: CreateVBO(UIndexBuffer *ib, UVBO_DRAW state){
	int size = ib->GetNum()*sizeof(unsigned int);
		
	GLuint vbo;
	OPENGL_CALL(glGenBuffers ( 1, &vbo ));
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER , vbo );
	OPENGL_CALL(glBufferData ( GL_ELEMENT_ARRAY_BUFFER , size, ib->GetPointer(), state ));		 
	
	return vbo;
}

void URenderer:: DeleteVBO(UBuffer *vb){
	GLuint vbo =  vb->GetId();
	OPENGL_CALL(glDeleteBuffers(1, &vbo));
}
void URenderer:: DrawBuffer(UVertexBuffer* vb){	
	OPENGL_CALL(glDrawArrays(GL_TRIANGLES, 0, vb->GetNum()));	
}
void URenderer:: DrawBuffer(UIndexBuffer* ib){	
	OPENGL_CALL(glDrawElements(GL_TRIANGLES, ib->GetNum(), GL_UNSIGNED_INT, NULL));	
}
void URenderer:: BindVBO(UVertexBuffer *vb){	
	glBindBuffer ( GL_ARRAY_BUFFER , vb->GetId());    
}
void URenderer:: BindVBO(UIndexBuffer *vb){
	
	if(previousIB != vb->GetId())
	{
		previousIB = vb->GetId();
		glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER , vb->GetId());
	}
}
void URenderer:: UnbindVBO(bool vertex_buffer){
	if(vertex_buffer){
		glBindBuffer ( GL_ARRAY_BUFFER , 0 ); 
	}else{
		glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER , 0 ); 
	}
}	

//VAO
int URenderer:: CreateVAO(){
	GLuint vao;
	OPENGL_CALL(glGenVertexArrays ( 1, &vao ));
    glBindVertexArray( vao );    	
	return vao;
}
void URenderer:: DeleteVAO(UVertexArrayObject *vao){
	GLuint id =  vao->GetId();
	OPENGL_CALL(glDeleteVertexArrays(1, &id));
}
void URenderer:: BindVAO(UVertexBuffer *vb){
	if(previousVAO != vb->GetVAO()->GetId())
	{
		previousVAO = vb->GetVAO()->GetId();
		glBindVertexArray(vb->GetVAO()->GetId());
	}
}
void URenderer:: UnbindVAO(){
	glBindVertexArray(0);    
}

//Shaders
int  URenderer:: CompileShader(std::string source, USHADER_TYPE st){
	
	GLuint shd;
	GLchar *strings = (GLchar*)source.c_str();

	shd = glCreateShader(st); 
	glShaderSource(shd, 1, (const GLchar**)&strings , NULL);
	glCompileShader(shd);
	
	GLint status;
	glGetShaderiv(shd, GL_COMPILE_STATUS, &status);
	if(status != GL_TRUE)
	{
		GLint infoLogLength;
		glGetShaderiv(shd, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shd, infoLogLength, NULL, strInfoLog);

		ULogger::GetInstance()->Message(string((char*)strInfoLog), ULOG_MSG_ERROR, ULOG_OUT_MSG);	
	}

	return shd;
}
void URenderer:: DeleteShader(UShader *shd){
	OPENGL_CALL(glDeleteShader(shd->GetId()));
}

int URenderer:: CreateShaderProgram(UShader *vertex_sh, UShader *pixel_sh){
	GLuint sh_pr_id = glCreateProgram();
	OPENGL_CALL(glAttachShader(sh_pr_id, vertex_sh->GetId()));
	OPENGL_CALL(glAttachShader(sh_pr_id, pixel_sh->GetId()));
	OPENGL_CALL(glLinkProgram(sh_pr_id));
	return sh_pr_id;
}

void URenderer:: DeleteShaderProgram(UShaderProgram *shd){
	OPENGL_CALL(glDeleteProgram(shd->GetId()));
}
void URenderer:: SetShaderProgram(UShaderProgram *sh){
	if(sh == nullptr || (shaderProgram != nullptr && sh == shaderProgram))
		return;
	shaderProgram = sh;
	OPENGL_CALL(glUseProgram(sh->GetId()));
	OPENGL_CHECK_FOR_ERRORS();
}
int URenderer:: CacheUniformLocation(string name)
{
	return CacheUniformLocation(name, shaderProgram);
}
int URenderer:: CacheUniformLocation(string name, UShaderProgram *sh)
{
	unsigned int *res = &uniformsCache[sh->GetId()][name];
	if(*res == 0)
	{
		unsigned int  loc = glGetUniformLocation(sh->GetId(),  name.c_str());
		
		if(loc > MAX_UNIFORM_LOCATIONS)
			loc = MAX_UNIFORM_LOCATIONS+1;
		*res = loc;
	}
	return *res;
}
void URenderer:: CacheUniform4(UShaderProgram *sh, std::string name, unsigned int num , float *variable){
	Uniform4(CacheUniformLocation(name, sh),  num, variable);
}
void URenderer:: CacheUniform4(std::string name, unsigned int num , float *variable){
	Uniform4(CacheUniformLocation(name),  num, variable);
}
void URenderer:: CacheUniformMatrix4(std::string name, unsigned int num , float *variable){	
	UniformMatrix4(CacheUniformLocation(name), num, variable);
}
void URenderer:: CacheUniformMatrix3(UShaderProgram *sh, std::string name, unsigned int num , float *variable){	
	UniformMatrix3(CacheUniformLocation(name, sh), num, variable);
}
void URenderer:: CacheUniformMatrix4(UShaderProgram *sh, std::string name, unsigned int num , float *variable){	
	UniformMatrix4(CacheUniformLocation(name, sh), num, variable);
}
void URenderer:: CacheUniformMatrix3(std::string name, unsigned int num , float *variable){	
	UniformMatrix3(CacheUniformLocation(name), num, variable);
}
void URenderer:: CacheUniform1(UShaderProgram *sh, std::string name, unsigned int num , float *variable){
	Uniform1(CacheUniformLocation(name, sh),  num, variable);
}
void URenderer:: CacheUniform1(std::string name, unsigned int num , float *variable){
	Uniform1(CacheUniformLocation(name),  num, variable);
}
void URenderer:: CacheUniform1(std::string name, int value){
	Uniform1(CacheUniformLocation(name), value);
}
void URenderer:: CacheUniform3(UShaderProgram *sh, std::string name, unsigned int num , float *variable){
	Uniform3(CacheUniformLocation(name, sh),  num, variable);
}
void URenderer:: CacheUniform3(std::string name, unsigned int num , float *variable){
	Uniform3(CacheUniformLocation(name),  num, variable);
}
///////////////

void URenderer:: Uniform4(unsigned int location, unsigned int num , float *variable){
	if(location < MAX_UNIFORM_LOCATIONS)
		glUniform4fv(location,  num, variable);
}
void URenderer:: UniformMatrix4(unsigned int location, unsigned int num , float *variable){	
	if(location < MAX_UNIFORM_LOCATIONS)
		glUniformMatrix4fv(location, num, GL_TRUE, variable);
}
void URenderer:: UniformMatrix3(unsigned int location, unsigned int num , float *variable){	
	if(location < MAX_UNIFORM_LOCATIONS)
		glUniformMatrix3fv(location, num, GL_TRUE, variable);
}
void URenderer:: Uniform1(unsigned int location, unsigned int num , float *variable){
	if(location < MAX_UNIFORM_LOCATIONS)
		glUniform1fv(location,  num, variable);
}
void URenderer:: Uniform1(unsigned int location, int value){
	if(location < MAX_UNIFORM_LOCATIONS)
		glUniform1i(location, value);
}
void URenderer:: Uniform3(unsigned int location, unsigned int num , float *variable){
	if(location < MAX_UNIFORM_LOCATIONS)
		glUniform3fv(location,  num, variable);
}

bool URenderer:: SetVerticalSynchronization(bool bEnabled)
{
	if(!wglSwapIntervalEXT)
		return false;
	if(bEnabled)
		wglSwapIntervalEXT(1);
	else
		wglSwapIntervalEXT(0);
	return true;
}

bool URenderer::Initialize(){
	//Create Window
	if( !uWnd.Create(L"UEngine",  std::atoi(UConfig::GetInstance()->GetParam("/xml/config/width/").c_str()),
								std::atoi(UConfig::GetInstance()->GetParam("/xml/config/height/").c_str()), 
								std::atoi(UConfig::GetInstance()->GetParam("/xml/config/fullscreen/").c_str()) == 1) )
		return false;
	
	//Initialize GL
	if(!InitExtensions())
		return false;
	
	OPENGL_CALL(glActiveTexture(GL_TEXTURE0));
	
	//Setup driver
	OPENGL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
	OPENGL_CALL(glClearDepth(1.0f));
	OPENGL_CALL(glEnable(GL_DEPTH_TEST));
	OPENGL_CALL(glEnable(GL_CULL_FACE));
	
	SetVerticalSynchronization(std::atoi(UConfig::GetInstance()->GetParam("/xml/config/vsync/").c_str()) == 1);

	OPENGL_CHECK_FOR_ERRORS();

	//Initialize camera
	float aspectRatio = (float)std::atoi(UConfig::GetInstance()->GetParam("/xml/config/width/").c_str()) /
							  (float)std::atoi(UConfig::GetInstance()->GetParam("/xml/config/height/").c_str());
	camera.Create(0.0f, 1.0f, 0.0f);
	camera.Perspective(45.0f, aspectRatio, 0.001f, 1000.0f);

	//Init IL
	DEVIL_CALL(ilInit());
	DEVIL_CALL(iluInit());
	//DEVIL_CALL(ilutInit());

	DEVIL_CHECK_FOR_ERRORS();
	
	//Print information
	PrintDebugInfo();

	return true;
}

void URenderer::Release(){
	uWnd.Destroy();	
}

void URenderer::PrintDebugInfo(){
	// Information about OpenGL
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

bool URenderer:: InitExtensions()
{
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
	OPENGL_GET_PROC(PFNGLDRAWBUFFERSPROC,			 glDrawBuffers);
	
	OPENGL_GET_PROC(PFNWGLSWAPINTERVALEXTPROC,		 wglSwapIntervalEXT);
	
	OPENGL_CHECK_FOR_ERRORS();

	return true;
}
