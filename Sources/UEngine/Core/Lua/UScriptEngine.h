#pragma once

#include "..\Basic\USingleton.hpp"
#include "..\Utils\ULogger.h"
#include "..\Utils\ExportDef.h"

#include "LuaTemplates.hpp"

class UScript;

class UScriptEngine : public USingleton<UScriptEngine>{

public:
	
	static UScriptEngine* GetInstance();
	
	static bool MessageBox(std::string text)
	{
		ULogger::GetInstance()->Message(text, ULOG_MSG_INFO, ULOG_OUT_MSG);
		return true;
	}

	void UENGINE_DECLSPEC RegisterFunctions(UScript* script);

	void UENGINE_DECLSPEC Release(){}

	UScriptEngine();
	~UScriptEngine();
};
