#include "UScriptEngine.h"

UScriptEngine:: UScriptEngine() {}

UScriptEngine:: ~UScriptEngine()
{
	ULogger::GetInstance()->Message("UScriptEngine singleton object is deleted...");
}

UScriptEngine* UScriptEngine:: GetInstance()
{
	if(instance == NULL){
		ULogger::GetInstance()->Message("UScriptEngine singleton object is created...");
		instance = new UScriptEngine();
	}
	return instance;
}