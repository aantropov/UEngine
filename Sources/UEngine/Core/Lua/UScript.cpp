#include "UScript.h"
#include "..\Resources\UXMLFile.h"
#include "UScriptEngine.h"
#include "..\utils\enum.h"

void UScript:: Initialize()
{
    luaVM = luaL_newstate();
    if (luaVM == nullptr)
    {
        return ULogger::GetInstance()->Message("Lua script cannot be initialized, cannot get new lua state", ULOG_MSG_ERROR);
    }
        
    luaL_openlibs(luaVM);    
    //lua_setglobal(luaVM, USCRIPT_GLOBAL_SCOPE);

    UScriptEngine::GetInstance()->RegisterFunctions(this);
}

void UScript:: Release()
{    
    callLua0(luaVM, USCRIPT_RELEASE_FUNCTION);
    if(luaVM != nullptr)
        lua_close(luaVM);
}

bool UScript:: Load(std::string path)
{    
    UXMLFile xml;    
    try
    {
        Initialize();
        xml.Load(path);
        this->name = xml.GetElement("/xml/script/name/");
        
        int file = luaL_loadfile(luaVM, xml.GetElement("/xml/script/lua/").c_str());
        if(file != 0)
        {
            ULogger::GetInstance()->Message("Cannot find file: " + xml.GetElement("/xml/script/lua/"));
            throw;
        }
        
        //execute file
        LUA_CALL(luaVM, lua_pcall(luaVM, 0, LUA_MULTRET, 0));                

    }catch(exception e){
        ULogger::GetInstance()->Message("Error to load script (xml): " + path, ULOG_MSG_ERROR, ULOG_OUT_MSG);
        return false;
    }
    return true;
}

void UScript:: Update(double delta)
{
    if(!inited)
    {
        callLua0(luaVM, USCRIPT_INIT_FUNCTION);
        inited = true;
    }
    callLua1(luaVM, USCRIPT_UPDATE_FUNCTION, (double)delta);
}
