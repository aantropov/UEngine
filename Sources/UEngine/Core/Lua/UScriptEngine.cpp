#include "UScriptEngine.h"
#include "UScript.h"

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

void UScriptEngine::RegisterFunctions(UScript* script)
{
	WrapFunc1<bool, std::string, UScriptEngine::MessageBoxW>().registerFunc(script->GetLuaVM(), "MessageBox");
}
