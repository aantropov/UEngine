#include "LuaTemplates.hpp"
#include <map>
#include <vector>

#include "..//math/UMath.h"
#include "..//GameMechanics//UGameMechanics.h"

using namespace std;


int g_LuaError = 0;

/////////////////////////////////////// basic types /////////////////////////////////////
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

////////////////////////////////// vec2 //////////////////////////////////////////////////
template<> void toLua(lua_State* L, vec2& arg)
{
  // stack:
  lua_newtable(L); // stack: table
  toLua(L, arg.x); // stack: table text
  lua_setfield(L, -2, "x"); // stack: table
  toLua(L, arg.y); // stack: table values
  lua_setfield(L, -2, "y"); // stack: table
}

template<> bool fromLua(lua_State* L, int index, vec2& arg)
{
  // stack:
  if(lua_istable(L, index))
  {
    float x;
	float y;

    if(fromLua(L, index, "x", x) && fromLua(L, index, "y", y)){
	  arg.x = x;
      arg.y = y;
      return true;
    }
  }
  return false;
}

////////////////////////////////// vec3 //////////////////////////////////////////////////
template<> void toLua(lua_State* L, vec3& arg)
{
  // stack:
  lua_newtable(L); // stack: table
  toLua(L, arg.x); // stack: table text
  lua_setfield(L, -2, "x"); // stack: table
  toLua(L, arg.y); // stack: table values
  lua_setfield(L, -2, "y"); // stack: table
  toLua(L, arg.z); // stack: table values
  lua_setfield(L, -2, "z"); // stack: table
}

template<> bool fromLua(lua_State* L, int index, vec3& arg)
{
  // stack:
  if(lua_istable(L, index))
  {
    float x;
	float y;
	float z;

    if(fromLua(L, index, "x", x) && fromLua(L, index, "y", y) && fromLua(L, index, "z", z)){
	  arg.x = x;
      arg.y = y;
	  arg.z = z;
      return true;
    }
  }
  return false;
}

////////////////////////////////// vec4 //////////////////////////////////////////////////
template<> void toLua(lua_State* L, vec4& arg)
{
  // stack:
  lua_newtable(L); // stack: table
  toLua(L, arg.x); // stack: table text
  lua_setfield(L, -2, "x"); // stack: table
  toLua(L, arg.y); // stack: table values
  lua_setfield(L, -2, "y"); // stack: table
  toLua(L, arg.z); // stack: table values
  lua_setfield(L, -2, "z"); // stack: table
  toLua(L, arg.w); // stack: table values
  lua_setfield(L, -2, "w"); // stack: table
}

template<> bool fromLua(lua_State* L, int index, vec4& arg)
{
  // stack:
  if(lua_istable(L, index))
  {
    float x;
	float y;
	float z;
	float w;

    if(fromLua(L, index, "x", x) && fromLua(L, index, "y", y)
		&& fromLua(L, index, "z", z) && fromLua(L, index, "w", w)){
	  arg.x = x;
      arg.y = y;
	  arg.z = z;
	  arg.w = w;
      return true;
    }
  }
  return false;
}

////////////////////////////////// quat //////////////////////////////////////////////////
template<> void toLua(lua_State* L, quat& arg)
{
  // stack:
  lua_newtable(L); // stack: table
  toLua(L, arg.x); // stack: table text
  lua_setfield(L, -2, "x"); // stack: table
  toLua(L, arg.y); // stack: table values
  lua_setfield(L, -2, "y"); // stack: table
  toLua(L, arg.z); // stack: table values
  lua_setfield(L, -2, "z"); // stack: table
  toLua(L, arg.w); // stack: table values
  lua_setfield(L, -2, "w"); // stack: table
}

template<> bool fromLua(lua_State* L, int index, quat& arg)
{
  // stack:
  if(lua_istable(L, index))
  {
    float x;
	float y;
	float z;
	float w;

    if(fromLua(L, index, "x", x) && fromLua(L, index, "y", y)
		&& fromLua(L, index, "z", z) && fromLua(L, index, "w", w)){
	  arg.x = x;
      arg.y = y;
	  arg.z = z;
	  arg.w = w;
      return true;
    }
  }
  return false;
}

////////////////////////////////// mat2 //////////////////////////////////////////////////
template<> void toLua(lua_State* L, mat2& arg)
{
  // stack:
  lua_newtable(L); // stack: table
  
  char buffer[MAXCHAR];
  memset(buffer, '\0', MAXCHAR * sizeof(char));
    
  for(unsigned int i = 0; i < 4; i++)
  {
	toLua(L, arg.m[i]);
	sprintf_s(buffer, "m%d", i+1);
	lua_setfield(L, -2, buffer);	
  }  
}

template<> bool fromLua(lua_State* L, int index, mat2& arg)
{
  // stack:
  if(lua_istable(L, index))
  {
	char buffer[MAXCHAR];
	memset(buffer, '\0', MAXCHAR * sizeof(char));

    float m[4];
	for(unsigned int i = 0; i < 4; i++)
	{
		sprintf_s(buffer, "m%d", i+1);
		if(!fromLua(L, index, buffer, m[i]))
			return false;
	}
	
	memcpy(arg.m, m, sizeof(arg.m[0]) * 4);
    return true;    
  }
  return false;
}

////////////////////////////////// mat3 //////////////////////////////////////////////////
template<> void toLua(lua_State* L, mat3& arg)
{
  // stack:
  lua_newtable(L); // stack: table
  
  char buffer[MAXCHAR];
  memset(buffer, '\0', MAXCHAR * sizeof(char));
    
  for(unsigned int i = 0; i < 9; i++)
  {
	toLua(L, arg.m[i]);
	sprintf_s(buffer, "m%d", i+1);
	lua_setfield(L, -2, buffer);	
  }  
}

template<> bool fromLua(lua_State* L, int index, mat3& arg)
{
  // stack:
  if(lua_istable(L, index))
  {
	char buffer[MAXCHAR];
	memset(buffer, '\0', MAXCHAR * sizeof(char));

    float m[9];
	for(unsigned int i = 0; i < 9; i++)
	{
		sprintf_s(buffer, "m%d", i+1);
		if(!fromLua(L, index, buffer, m[i]))
			return false;
	}
	
	memcpy(arg.m, m, sizeof(arg.m[0]) * 9);
    return true;    
  }
  return false;
}

////////////////////////////////// mat4 //////////////////////////////////////////////////
template<> void toLua(lua_State* L, mat4& arg)
{
  // stack:
  lua_newtable(L); // stack: table
  
  char buffer[MAXCHAR];
  memset(buffer, '\0', MAXCHAR * sizeof(char));
    
  for(unsigned int i = 0; i < 16; i++)
  {
	toLua(L, arg.m[i]);
	sprintf_s(buffer, "m%d", i+1);
	lua_setfield(L, -2, buffer);	
  }  
}

template<> bool fromLua(lua_State* L, int index, mat4& arg)
{
  // stack:
  if(lua_istable(L, index))
  {
	char buffer[MAXCHAR];
	memset(buffer, '\0', MAXCHAR * sizeof(char));

    float m[16];
	for(unsigned int i = 0; i < 16; i++)
	{
		sprintf_s(buffer, "m%d", i+1);
		if(!fromLua(L, index, buffer, m[i]))
			return false;
	}
	
	memcpy(arg.m, m, sizeof(arg.m[0]) * 16);
    return true;    
  }
  return false;
}

////////////////////////////////// transform //////////////////////////////////////////////////
template<> void toLua(lua_State* L, transform& arg)
{
  // stack:
  lua_newtable(L); // stack: table
  toLua(L, arg.pos); // stack: table text
  lua_setfield(L, -2, "pos"); // stack: table
  toLua(L, arg.rotation); // stack: table values
  lua_setfield(L, -2, "rotation"); // stack: table
  toLua(L, arg.scale); // stack: table values
  lua_setfield(L, -2, "scale"); // stack: table
}

template<> bool fromLua(lua_State* L, int index, transform& arg)
{
  // stack:
  if(lua_istable(L, index))
  {
    vec4 p;
	quat r;
	vec4 s;

    if(fromLua(L, index, "pos", p) && fromLua(L, index, "rotation", r)
		&& fromLua(L, index, "scale", s)){
	  
	  r = normalize(r);
	  arg.pos = p;
	  arg.rotation = r;
	  arg.scale = s;

      return true;
    }
  }
  return false;
}

////////////////////////////////// objects //////////////////////////////////////////////////
template<> bool fromLua ( lua_State * lua, int index, void*& ret)
{
    if( !lua_isnumber ( lua, index ))
        return false;
 
	void* temp;

	try{
		int number = (int)lua_tonumber ( lua, index );
		temp = reinterpret_cast<void*>(number);
	}catch(std::exception ex){return false;}
	
	ret = temp;

    return true;
}

template<> bool fromLua ( lua_State * lua, int index, UGameObject*& ret)
{
	return fromLua(lua, index, (void*&) ret);
}

template<> bool fromLua ( lua_State * lua, int index, UComponent*& ret)
{
	return fromLua(lua, index, (void*&) ret);
}

template<> void toLua ( lua_State * lua, void*& arg )
{
    lua_pushnumber ( lua, reinterpret_cast<int>(arg));
}

template<> void toLua ( lua_State * lua, UGameObject*& arg )
{
    toLua(lua, (void*&)arg);
}

template<> void toLua ( lua_State * lua, UComponent*& arg )
{
    toLua(lua, (void*&)arg);
}
