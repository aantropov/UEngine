//
// Simple templated Lua wrapper.
// Based on code from http://ilovelua.narod.ru
//

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

#include    <string>
#include    <map>
#include    <vector>

#include    "..\Utils\enum.h"
#include    "..\Utils\ULogger.h"

extern int g_LuaError;
#ifdef UE_DEBUG
#define LUA_CALL(state, expression) \
	{ \
		if ((g_LuaError = expression) != 0) \
		{ \
			char message[UE_MAXCHAR]; \
			sprintf_s(message,"Lua expression \"" #expression "\" Error: %s\n", lua_tostring(state, -1)); \
			ULogger::GetInstance()->Message(message, ULOG_MSG_ERROR, ULOG_OUT_MSG); \
		} \
	}
#endif //UE_DEBUG

#ifdef UE_RELEASE
#define LUA_CALL(state, expression) expression;
#endif //UE_RELEASE

// transfer values to Lua and from Lua
template<typename T> bool fromLua ( lua_State * lua, int index, T& ret );
template<typename T> void toLua   ( lua_State * lua, T& arg );

// take value from Lua table
template<typename T>
bool fromLua ( lua_State * lua, int index, const char * key, T& ret)
{
	(index ? lua_getfield(lua, index, key) : lua_getglobal(lua, key));

    bool res = fromLua ( lua, -1, ret);
    lua_pop ( lua, 1 ); // stack: table

    return res;
}

//////////////////////////////////std::vector////////////////////////////////////////

template<typename T>
bool fromLua(lua_State* lua, int index, std::vector<T>& ret)
{	
    if(!lua_istable ( lua, index))
        return false;

    lua_pushvalue ( lua, index );       // stack: map
    lua_pushnil   ( lua );              // stack: map nil
    
	ret.clear();

    while(lua_next ( lua, -2 ))         // stack: map key value
    {
        T	value;		        
        fromLua (lua, -1, value);        
        ret.push_back(value);        
        lua_pop ( lua, 1 );             // stack: map key
    }
    
    lua_pop ( lua, 1);                  // stack:

  return true;
}

template<typename T>
void toLua(lua_State* L, std::vector<T>& arg)
{
  const int size = arg.size();
  //lua_pushnumber(L, size);

  lua_newtable(L); // stack: table

  for(unsigned int i = 0; arg.size() > i; ++i)
  {
    lua_pushnumber(L, i + 1); // stack: table i
    toLua(L, arg[i]); // stack: table i value
    lua_settable(L, -3); // stack: table
  }
}

//////////////////////////////////std::map//////////////////////////////////////////
template<typename TKey, typename TValue>
void toLua(lua_State* L, std::map<TKey, TValue>& arg)
{
  // stack:
  lua_newtable(L); // stack: table
  for(typename std::map<TKey, TValue>::const_iterator i = arg.begin(); arg.end() != i; ++i)
  {
    toLua(L, i->first); // stack: table key
    toLua(L, i->second); // stack: table key value
    lua_settable(L, -3); // stack: table
  }
}

template<typename Key, typename Value>
bool fromLua ( lua_State * lua, int index, std::map<Key, Value>& ret)
{	
    if(!lua_istable ( lua, index))
        return false;

    lua_pushvalue ( lua, index );       // stack: map
    lua_pushnil   ( lua );              // stack: map nil
    
	ret = std::map<Key, Value>();

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

/////////////// templates to call Lua functions passsing arguments /////////////////////
template <typename R> 
static inline bool callLua0 ( lua_State * lua, const char * funcName, R& res )
{
	
    lua_getglobal( lua, funcName);   // push global function f on stack
    LUA_CALL(lua, lua_pcall( lua, 0, LUA_MULTRET, 0 ));                      // call function taking no argsuments and getting one return value
    
	fromLua      ( lua, -1, res );
	lua_pop      ( lua, 1 );
    
    return true;
}

static inline void callLua0 ( lua_State * lua, const char * funcName )
{
    lua_getglobal( lua, funcName);   // push global function f on stack
    LUA_CALL(lua, lua_pcall( lua, 0, LUA_MULTRET, 0 ));                      // call function taking no argsuments and getting one return value
}

template <typename R, typename A> 
static inline bool    callLua1 ( lua_State * lua, const char * funcName, R& res, A arg )
{

    lua_getglobal( lua, funcName);   // push global function f on stack
    toLua        ( lua, arg );                          // push first argument on stack
    LUA_CALL(lua, lua_pcall( lua, 1, LUA_MULTRET, 0 ));                      // call function taking 1 argsuments and getting one return value
    fromLua      ( lua, -1, res );
	lua_pop      ( lua, 1 );
    
    return true;
}

template <typename A> 
static inline void    callLua1 ( lua_State * lua, const char * funcName, A arg )
{
    lua_getglobal( lua, funcName);   // push global function f on stack
    toLua        ( lua, arg );                          // push first argument on stack
    LUA_CALL(lua, lua_pcall( lua, 1, LUA_MULTRET, 0 ));                      // call function taking 1 argsuments and getting one return value
}

template <typename R, typename A1, typename A2> 
static inline bool    callLua2 ( lua_State * lua, const char * funcName, R& res, A1 arg1, A2 arg2 )
{
	
    lua_getglobal( lua, funcName);   // push global function f on stack
    toLua        ( lua, arg1 );                         // push first argument on stack
    toLua        ( lua, arg2 );                         // push 2nd argument on stack
    LUA_CALL(lua, lua_pcall( lua, 2, LUA_MULTRET, 0 ));                      // call function taking 2 argsuments and getting one return value
    fromLua      ( lua, -1, res );
	lua_pop      ( lua, 1 );
    
    return true;
}

template <typename A1, typename A2> 
static inline void    callLua2 ( lua_State * lua, const char * funcName, A1 arg1, A2 arg2 )
{
    lua_getglobal( lua, funcName);   // push global function f on stack
    toLua        ( lua, arg1 );                         // push first argument on stack
    toLua        ( lua, arg2 );                         // push 2nd argument on stack
    LUA_CALL(lua, lua_pcall( lua, 2, LUA_MULTRET, 0 ));                      // call function taking 2 argsuments and getting one return value
}

template <typename R, typename A1, typename A2, typename A3> 
static inline bool    callLua3 ( lua_State * lua, const char * funcName, R& res, A1 arg1, A2 arg2, A3 arg3 )
{
    lua_getglobal( lua, funcName);   // push global function f on stack
    toLua        ( lua, arg1 );                         // push first argument on stack
    toLua        ( lua, arg2 );                         // push 2nd argument on stack
    toLua        ( lua, arg3 );                         // push 3rdt argument on stack
    LUA_CALL(lua, lua_pcall( lua, 2, LUA_MULTRET, 0 ));                      // call function taking 2 argsuments and getting one return value
    fromLua      ( lua, -1, res );
	lua_pop      ( lua, 1 );
    
    return true;
}

template <typename A1, typename A2, typename A3> 
static inline void    callLua3 ( lua_State * lua, const char * funcName, A1 arg1, A2 arg2, A3 arg3 )
{
    lua_getglobal( lua, funcName);   // push global function f on stack
    toLua        ( lua, arg1 );                         // push first argument on stack
    toLua        ( lua, arg2 );                         // push 2nd argument on stack
    toLua        ( lua, arg3 );                         // push 3rdt argument on stack
    LUA_CALL(lua, lua_pcall( lua, 2, LUA_MULTRET, 0 ));                      // call function taking 2 argsuments and getting one return value
}

///////////////////////// template to wrap function for Lua /////////////////////

template <typename R, R (*func)()>
class   WrapFunc0
{
public:
    static int f ( lua_State * lua )
    {
        R   r = func ();
        
        toLua   ( lua, r );
        
        return 1;                                       // return one vaue
    }
    
    void    registerFunc ( lua_State * lua, const char * name )
    {
        lua_register ( lua, name, f );
    }
};

template <typename R, typename A, R (*func)(A)>
class   WrapFunc1
{
public:
    static int f ( lua_State * lua )
    {
        A   a;
        R   r;
        
        fromLua ( lua, 1, a );
        
        r = func ( a );
        
        toLua   ( lua, r );
        
        return 1;                                       // return one vaue
    }
    
    void    registerFunc ( lua_State * lua, const char * name )
    {
        lua_register ( lua, name, f );
    }
};

template <typename R, typename A1, typename A2, R (*func)(A1,A2)>
class   WrapFunc2
{
public:
    static int f ( lua_State * lua )
    {
        A1  a1;
        A2  a2;
        R   r;
        
        fromLua ( lua, 1, a1 );
        fromLua ( lua, 2, a2 );
        
        r = func ( a1, a2 );
        
        toLua   ( lua, r );
        
        return 1;                                       // return one vaue
    }
    
    void    registerFunc ( lua_State * lua, const char * name )
    {
        lua_register ( lua, name, f );
    }
};

template <typename R, typename A1, typename A2, typename A3, R (*func)(A1,A2,A3)>
class   WrapFunc3
{
public:
    static int f ( lua_State * lua )
    {
        A1  a1;
        A2  a2;
        A3  a3;
        R   r;
        
        fromLua ( lua, 1, a1 );
        fromLua ( lua, 2, a2 );
        fromLua ( lua, 3, a3 );
        
        r = func ( a1, a2, a3 );
        
        toLua   ( lua, r );
        
        return 1;                                       // return one vaue
    }
    
    void    registerFunc ( lua_State * lua, const char * name )
    {
        lua_register ( lua, name, f );
    }
};
