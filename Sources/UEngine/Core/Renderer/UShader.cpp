#include "UShader.h"
#include "urenderer.h"

//Compile Shader
void UShader::Create(USHADER_TYPE st){
	_id = URenderer::GetInstance()->CompileShader(source, st);
}
//Load from file source code of the shader
bool UShader::Load(std::string path) {
	FILE* fShd;
	//Safe opening
	fopen_s(&fShd, path.c_str() , "rb");

	if(fShd == NULL){		
		ULogger::GetInstance()->Message("Shader: Can`t open file " + path, ULOG_MSG_ERROR);	
		return false;
	}
	while(!feof(fShd)){
		char temp = '\0';
		fread(&temp, 1, 1, fShd);
		source.push_back(temp);
	}	

	if(fShd != NULL){
		fclose(fShd);
	//	ULogger::GetInstance()->Message("Shader: " + path + " is closed");
	}
	return true;
}
//Delete all data
void UShader::Free(){
	if(_id != -1)
		URenderer::GetInstance()->DeleteShader(this);
	_id = -1;
	source.clear();
}