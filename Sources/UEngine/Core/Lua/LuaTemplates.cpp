#include "LuaTemplates.h"

int g_LuaError = 0;

template<> bool fromLua ( lua_State * lua, int index, bool& ret )
{
    if( !lua_isboolean ( lua, index ) )
        return false;
        
    ret = lua_toboolean ( lua, index) != 0;
    return true;
}

template<> bool fromLua ( lua_State * lua, int index, float& ret)
{
    if( !lua_isnumber ( lua, index ) )
        return false;
        
    ret = (float) lua_tonumber ( lua, index );
    return true;
}

template<> bool fromLua ( lua_State * lua, int index, double& ret)
{
    if(!lua_isnumber ( lua, index ))
        return false;
        
    ret = lua_tonumber ( lua, index );
    return true;
}

template<> bool fromLua ( lua_State * lua, int index, int& ret)
{
    if( !lua_isnumber ( lua, index ))
        return false;
 
    ret = (int)lua_tonumber ( lua, index );
    return true;
}

template<> bool fromLua(lua_State * lua, int index, std::string& ret)
{
    if( !lua_isstring ( lua, index))
        return false;
        
    ret = lua_tostring ( lua, index);
    return true;
}

template<typename Key, typename Value>
bool fromLua ( lua_State * lua, int index, std::map<Key, Value>& ret)
{
    if(!lua_istable ( lua, index))
        return false;

    lua_pushvalue ( lua, index );       // stack: map
    lua_pushnil   ( lua );              // stack: map nil
    
    while(lua_next ( lua, -2 ))         // stack: map key value
    {
        Key     key;
        Value   value;
        
        fromLua ( lua, -2, key );
        fromLua ( lua, -1, value);
        
        ret [key] = value;
        
        lua_pop ( lua, 1 );             // stack: map key
    }
    
    lua_pop ( lua, 1);                  // stack:

  return true;
}

template<typename T>
bool fromLuaTable ( lua_State * lua, int index, const char * key, T& ret)
{
    lua_getfield ( lua, index? index : lua_getglobal(lua, USCRIPT_GLOBAL_SCOPE), key ); // stack: table value

    bool res = fromLua ( lua, -1, ret);

    lua_pop ( lua, 1 ); // stack: table

    return res;
}

template<> void toLua ( lua_State * lua, const bool& arg )
{
    lua_pushboolean ( lua, arg ? 1 : 0 );
}

template<> void toLua ( lua_State * lua, bool& arg )
{
    lua_pushboolean ( lua, arg ? 1 : 0 );
}

template<> void toLua ( lua_State * lua, const float& arg )
{
    lua_pushnumber ( lua, arg );
}

template<> void toLua ( lua_State * lua, float& arg )
{
    lua_pushnumber ( lua, arg );
}

template<> void toLua ( lua_State * lua, const double& arg )
{
    lua_pushnumber ( lua, arg );
}

template<> void toLua ( lua_State * lua, double& arg )
{
    lua_pushnumber ( lua, arg );
}

template<> void toLua ( lua_State * lua, const int& arg )
{
    lua_pushnumber ( lua, arg );
}

template<> void toLua ( lua_State * lua, int& arg )
{
    lua_pushnumber ( lua, arg );
}

template<> void toLua ( lua_State * lua, const std::string& arg )
{
    lua_pushstring ( lua, arg.c_str () );
}

template<> void toLua ( lua_State * lua, std::string& arg )
{
    lua_pushstring ( lua, arg.c_str () );
}
