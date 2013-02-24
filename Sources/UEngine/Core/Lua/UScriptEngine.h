#pragma once

#include "..\Basic\USingleton.hpp"
#include "..\Utils\ULogger.h"
#include "..\Utils\ExportDef.h"

#include "LuaTemplates.h"

class UScript;

class UScriptEngine : public USingleton<UScriptEngine>{

public:
	
	static UScriptEngine* GetInstance();

	void UENGINE_DECLSPEC RegisterFunctions(UScript* script){}
	void UENGINE_DECLSPEC Release(){}

	UScriptEngine();
	~UScriptEngine();
};
