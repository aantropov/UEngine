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

#include "..\GameMechanics\UComponent.h"
#include "..\Utils\ULogger.h"
#include "..\Resources\UResource.h"

class UScript :    public UComponent, public UResource{

    lua_State *luaVM;
    bool inited;

public:
    
    UGameObject* GetGameObject(){ return gameObject; }

    lua_State* GetLuaVM(){ return luaVM;}

    void UENGINE_DECLSPEC Initialize();
    void UENGINE_DECLSPEC Release();

    virtual void Update(double delta);
    
    virtual bool Load(std::string path);
    virtual void Free() { Release(); }

    UScript(): luaVM(nullptr), inited(false) {}
    ~UScript(){ Free(); }
};

