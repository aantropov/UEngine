#pragma once

extern "C"
{
	#include "lua 5.2.1\lua.h"
	#include "lua 5.2.1\lualib.h"
	#include "lua 5.2.1\lauxlib.h"
	#include "lua 5.2.1\luaconf.h"
	#include "lua 5.2.1\lua.hpp"
}

#pragma comment(lib, "lua52.lib")

#include "..\Basic\USingleton.hpp"
#include "..\Utils\ULogger.h"
#include "..\Utils\ExportDef.h"

class UScriptEngine : public USingleton<UScriptEngine>{
	
	lua_State *luaVM;

public:
	
	static UScriptEngine* GetInstance();

	void UENGINE_DECLSPEC Initialize()
	{
		luaVM = luaL_newstate();
 
		if (NULL == luaVM)
		{
			return ULogger::GetInstance()->Message("Lua cannot be initialized, cannot get new lua state", ULOG_MSG_ERROR);
		}

		luaL_openlibs(luaVM);
		
		//char* strLuaInput = "a = 1 + 1;\nprint( a);\n"; 
		//lua_dostring(luaVM, strLuaInput);
		
	}

	void UENGINE_DECLSPEC Release()
	{
		//lua_close(luaVM);
	}

	void Run();

	UScriptEngine();
	~UScriptEngine();
};