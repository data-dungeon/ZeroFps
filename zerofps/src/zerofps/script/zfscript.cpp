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
	Open();
}

ZFScript::~ZFScript()
{
	Close();
}

///////////////////////////////////////////////////////////////////////////////
// Name:		OpenLua
// Description:	Starta lua, �ppna vissa hj�lpfunktioner och skapa ett alla de
//				tag metoder som kr�vs f�r att kunna registrera globala variabler
//				till Lua.
//
bool ZFScript::Open()
{
	// Open Lua
	m_pkLua = lua_open(0);

	// Open base lib for access to some useful functions.
//	lua_baselibopen(m_pkLua);

	// Create Lua tag for Int type.
	m_iLuaTagInt = lua_newtag(m_pkLua);
	lua_pushcfunction(m_pkLua, GetTypeInt);
	lua_settagmethod(m_pkLua, m_iLuaTagInt, "getglobal");
	lua_pushcfunction(m_pkLua, SetTypeInt); 
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
	lua_pushcfunction(m_pkLua, GetTypeString);
	lua_settagmethod(m_pkLua, m_iLuaTagString, "getglobal");
	lua_pushcfunction(m_pkLua, SetTypeString); 
	lua_settagmethod(m_pkLua, m_iLuaTagString, "setglobal");

	return true;	
}

void ZFScript::Close()
{
	lua_close(m_pkLua);
}

///////////////////////////////////////////////////////////////////////////////
// Name:		RunScript
// Description:	K�r ett script fr�n en fil.
//
bool ZFScript::RunScript(char* szFileName)
{
	printf("SCRIPT_API: Executing script %s\n", szFileName);
	return (lua_dofile(m_pkLua, szFileName) == 0);
}

///////////////////////////////////////////////////////////////////////////////
// Name:		RunScript
// Description:	K�r ett script fr�n en fil.
//
bool ZFScript::CallScript(char* szFuncName, int iNumParams, int iNumResults)
{
	printf("SCRIPT_API: Calling script function %s\n", szFuncName);
	lua_getglobal( m_pkLua, szFuncName);
	return (lua_call(m_pkLua, iNumParams, iNumResults) == 0);
}

///////////////////////////////////////////////////////////////////////////////
// Name:		RegisterClass
// Description:	Registrera en C++ klass som Lua kan se.
//
bool ZFScript::ExposeClass(char *szName, ScripObjectType eType, 
						   lua_CFunction o_LuaGet, 
						   lua_CFunction o_LuaSet)
{
	// check if type is already registered
	bool bAlreadyExist = true;
	if(m_kExposedClasses.find(string(szName)) == m_kExposedClasses.end())
		bAlreadyExist = false;

	if(bAlreadyExist)
	{
		printf("SCRIPT_API: Class [%s] already exposed %s\n", szName);
		return true;
	}

	// Create Lua tag for Test type.
	lua_pushcfunction(m_pkLua, o_LuaGet);
	lua_settagmethod(m_pkLua, tolua_tag(m_pkLua,szName), "getglobal");

	lua_pushcfunction(m_pkLua, o_LuaSet); 
	lua_settagmethod(m_pkLua, tolua_tag(m_pkLua,szName), "setglobal");
 
	m_kExposedClasses.insert(string(szName));

	// Register class
	m_kClassMap.insert( map<ScripObjectType,string>::value_type(eType, string(szName)) );

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name:		ExposeClass
// Description:	Registrera en C++ klass som Lua kan se.
//
bool ZFScript::ExposeObject(const char* szName, void* pkData, ScripObjectType eType)
{

	map<ScripObjectType, string>::iterator itClass;
	itClass = m_kClassMap.find(eType);
	
	// no such element exists
	if(itClass == m_kClassMap.end())
	{
		printf("SCRIPT_API: Error accessing class object!");
		return false;
	}

	char *szClassName = (char*) itClass->second.c_str();

	lua_pushusertag(m_pkLua, pkData, tolua_tag(m_pkLua, szClassName));
	lua_setglobal(m_pkLua, szName);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name:		ExposeFunction
// Description:	Registrera en C++ function som Lua kan se.
//
bool ZFScript::ExposeFunction(const char *szName, lua_CFunction o_Function)
{
	lua_register( m_pkLua, szName, o_Function );
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name:		ExposeVariable
// Description:	Registrera en C++ variabel som Lua kan se.
//
bool ZFScript::ExposeVariable(const char* szName, void* pkData, ScripVarType eType)
{
	switch(eType)
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

///////////////////////////////////////////////////////////////////////////////
// Ett g�ng statiska funktioner som triggas fr�n Lua n�r man beh�ver komma �t
// en global c++ variabel fr�n skript (som i f�rv�g har blivit exponerad).
//
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


