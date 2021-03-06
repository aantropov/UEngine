#include "UScript.h"
#include "..\Resources\UXMLFile.h"
#include "UScriptEngine.h"
#include "..\utils\enum.h"

void UScript::Initialize()
{
    luaVM = luaL_newstate();
    if (luaVM == nullptr)
    {
        return ULogger::GetInstance()->Message("Lua script cannot be initialized, cannot get new lua state", ULogType::Error);
    }

    luaL_openlibs(luaVM);
    //lua_setglobal(luaVM, USCRIPT_GLOBAL_SCOPE);

    UScriptEngine::GetInstance()->RegisterFunctions(this);
}

void UScript::Release()
{
    callLua0(luaVM, USCRIPT_RELEASE_FUNCTION);
    if (luaVM != nullptr)
        lua_close(luaVM);
}

bool UScript::Load(UXMLFile& xml, std::string path)
{
    try
    {
        Initialize();
        this->name = xml.GetElement(path + "script/name/");

        int file = luaL_loadfile(luaVM, xml.GetElement(path + "script/lua/").c_str());
        if (file != 0)
        {
            ULogger::GetInstance()->Message("Cannot find file: " + xml.GetElement(path + "script/lua/"));
            throw;
        }

        //execute file
        LUA_CALL(luaVM, lua_pcall(luaVM, 0, LUA_MULTRET, 0));

    }
    catch (exception e) {
        ULogger::GetInstance()->Message("Error to load script (xml): " + path, ULogType::Error, ULogTarget::MsgBox);
        return false;
    }
    return true;
}

void UScript::Update(double delta)
{
    if (!inited)
    {
        callLua0(luaVM, USCRIPT_INIT_FUNCTION);
        inited = true;
    }
    callLua1(luaVM, USCRIPT_UPDATE_FUNCTION, (double)delta);
}
