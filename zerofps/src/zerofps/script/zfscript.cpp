// zfscript.cpp: implementation of the ZFScript class.
//
//////////////////////////////////////////////////////////////////////

#include "zfscript.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZFScript::ZFScript()
{
	OpenLua();
}

ZFScript::~ZFScript()
{
	lua_close(m_pkLua);
}

bool ZFScript::OpenLua()
{
	m_pkLua = lua_open(0);
	lua_baselibopen(m_pkLua);

	// Create Lua tag for Int type.
	m_iLuaTagInt = lua_newtag(m_pkLua);
	lua_pushcfunction(m_pkLua, SetTypeInt);
	lua_settagmethod(m_pkLua, m_iLuaTagInt, "getglobal");
	lua_pushcfunction(m_pkLua, GetTypeInt); 
	lua_settagmethod(m_pkLua, m_iLuaTagInt, "setglobal");

	// Create Lua tag for Double type.
	m_iLuaTagDouble = lua_newtag(m_pkLua);
	lua_pushcfunction(m_pkLua, GetTypeDouble);
	lua_settagmethod(m_pkLua, m_iLuaTagDouble, "getglobal");
	lua_pushcfunction(m_pkLua, SetTypeDouble); 
	lua_settagmethod(m_pkLua, m_iLuaTagDouble, "setglobal");

	// Create Lua tag for Float type.
	m_iLuaTagFloat = lua_newtag(m_pkLua);
	lua_pushcfunction(m_pkLua, GetTypeFloat);
	lua_settagmethod(m_pkLua, m_iLuaTagFloat, "getglobal");
	lua_pushcfunction(m_pkLua, SetTypeFloat); 
	lua_settagmethod(m_pkLua, m_iLuaTagFloat, "setglobal");

	// Create Lua tag for String type.
	m_iLuaTagString = lua_newtag(m_pkLua);
	lua_pushcfunction(m_pkLua, SetTypeString);
	lua_settagmethod(m_pkLua, m_iLuaTagString, "getglobal");
	lua_pushcfunction(m_pkLua, GetTypeString); 
	lua_settagmethod(m_pkLua, m_iLuaTagString, "setglobal");

	return true;	
}

bool ZFScript::RunScript(char* szFileName)
{
	printf("SCRIPT_API: Executing script %s\n", szFileName);
	return (lua_dofile(m_pkLua, szFileName) == 0);
}

bool ZFScript::ExposeVariable(const char* szName, void* pkData, VarType eVariableType)
{
	switch(eVariableType)
	{
	case tINT:
		lua_pushusertag(m_pkLua, pkData, m_iLuaTagInt);
		lua_setglobal(m_pkLua, szName);
		break;
	case tDOUBLE:
		lua_pushusertag(m_pkLua, pkData, m_iLuaTagDouble);
		lua_setglobal(m_pkLua, szName);
		break;
	case tFLOAT:
		lua_pushusertag(m_pkLua, pkData, m_iLuaTagFloat);
		lua_setglobal(m_pkLua, szName);
		break;
	case tSTRING:
		lua_pushusertag(m_pkLua, pkData, m_iLuaTagString);
		lua_setglobal(m_pkLua, szName);
		break;
	}

	return true;
}

// Int
int ZFScript::SetTypeInt(lua_State* pkLua) {
	int* var=(int*) lua_touserdata(pkLua,2);
	int  val=(int)  lua_tonumber(pkLua,3);
	*var=val;
	return 0;
}
int ZFScript::GetTypeInt(lua_State* pkLua) {
	int* var=(int*) lua_touserdata(pkLua,2);
	lua_pushnumber(pkLua,*var);
	return 1;
}

// Double
int ZFScript::SetTypeDouble(lua_State* pkLua) {
	double* var=(double*) lua_touserdata(pkLua,2);
	double  val=(double)  lua_tonumber(pkLua,3);
	*var=val;
	return 0;
}
int ZFScript::GetTypeDouble(lua_State* pkLua) {
	double* var=(double*) lua_touserdata(pkLua,2);
	lua_pushnumber(pkLua,*var);
	return 1;
}

// Float
int ZFScript::SetTypeFloat(lua_State* pkLua) {
	float* var=(float*) lua_touserdata(pkLua,2);
	float  val=(float)  lua_tonumber(pkLua,3);
	*var=val;
	return 0;
}
int ZFScript::GetTypeFloat(lua_State* pkLua) {
	float* var=(float*) lua_touserdata(pkLua,2);
	lua_pushnumber(pkLua,*var);
	return 1;
}

// String
int ZFScript::SetTypeString(lua_State* pkLua) {
	char* var= (char*) lua_touserdata(pkLua,2);
	char* val= (char*) lua_tostring(pkLua,3);
	var=val;
	return 0;
}
int ZFScript::GetTypeString(lua_State* pkLua) {
	char* var=(char*) lua_touserdata(pkLua,2);
	lua_pushstring(pkLua,var);
	return 1;
}


