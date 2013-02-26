#include "LuaTemplates.hpp"
#include <map>
#include <vector>

using namespace std;

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

template<typename T>
bool fromLua(lua_State* lua, int index, std::vector<T>& ret)
{
  // stack:
  if(!lua_istable(lua, index))
    return false;

  lua_pushvalue(L, index-1);
  lua_pushvalue(L, index); // stack: vector

  int count;
  fromLua ( lua, -2, count);

  for(int i = 1; count >= i; ++i)
  {
    lua_pushnumber(L, i);
    lua_gettable(L, -2);
    T value;
    fromLua(L, -1, value);
    ret.push_back(value);
    lua_pop(L, 1); // stack: vector
  }
  lua_pop(L, 2); // stack:

  return true;
}

template<typename T>
bool fromLuaTable ( lua_State * lua, int index, const char * key, T& ret)
{
	(index ? lua_getfield(lua, index, key) : lua_getglobal(lua, key));

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

template<typename T>
void toLua(lua_State* L, const std::vector<T>& arg)
{
  const int size = arg.size();
  lua_pushnumber(L, size);

  lua_newtable(L); // stack: table

  for(int i = 0; arg.size() > i; ++i)
  {
    lua_pushnumber(L, i + 1); // stack: table i
    toLua(L, arg[i]); // stack: table i value
    lua_settable(L, -3); // stack: table
  }
}
