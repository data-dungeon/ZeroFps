// gamescript.cpp: implementation of the GameScript class.
//
//////////////////////////////////////////////////////////////////////

#include "gamescript.h"

// package files
#include "../engine/engine.pkg"
#include "package_files/zfscript_bind.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GameScript::GameScript() : ZFScript()
{
	OpenPackageFiles();
}

GameScript::~GameScript()
{

}

static int LuaGetBasicConsole(lua_State* pkLua) 
{
	BasicConsole* var=(BasicConsole*) lua_touserdata(pkLua,2);
	lua_pushusertag(pkLua, var, tolua_tag(pkLua, "BasicConsole"));
	return 1;
}

static int LuaSetBasicConsole(lua_State* pkLua) 
{
	BasicConsole* var=(BasicConsole*) lua_touserdata(pkLua,2);
	BasicConsole* val=(BasicConsole*) lua_touserdata(pkLua,3);
	var=val;
	return 0;
}

static int LuaGetVector3(lua_State* pkLua) 
{
	Vector3* var=(Vector3*) lua_touserdata(pkLua,2);
	lua_pushusertag(pkLua, var, tolua_tag(pkLua, "Vector3"));
	return 1;
}

static int LuaSetVector3(lua_State* pkLua) 
{
	Vector3* var=(Vector3*) lua_touserdata(pkLua,2);
	Vector3* val=(Vector3*) lua_touserdata(pkLua,3);
	var=val;
	return 0;
}

static int LuaGetEngineScriptInterface(lua_State* pkLua) 
{
	EngineScriptInterface* var=(EngineScriptInterface*) lua_touserdata(pkLua,2);
	lua_pushusertag(pkLua, var, tolua_tag(pkLua, "EngineScriptInterface"));
	return 1;
}

static int LuaSetEngineScriptInterface(lua_State* pkLua) 
{
	EngineScriptInterface* var=(EngineScriptInterface*) lua_touserdata(pkLua,2);
	EngineScriptInterface* val=(EngineScriptInterface*) lua_touserdata(pkLua,3);
	var=val;
	return 0;
}

void GameScript::OpenPackageFiles()
{	
	tolua_zfscript_bind_open(GetLua());

	ExposeClass("Console", tConsole, LuaGetBasicConsole, LuaSetBasicConsole);
	ExposeClass("Vector3", tVector3, LuaGetVector3, LuaSetVector3);
	ExposeClass("EngineScriptInterface", tEngine, LuaGetEngineScriptInterface, LuaSetEngineScriptInterface);
}



