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

static int LuaSetBasicConsole(lua_State* pkLua) 
{
	BasicConsole* var=(BasicConsole*) lua_touserdata(pkLua,2);
	BasicConsole* val=(BasicConsole*) lua_touserdata(pkLua,3);
	var=val;
	return 0;
}

static int LuaGetBasicConsole(lua_State* pkLua) 
{
	BasicConsole* var=(BasicConsole*) lua_touserdata(pkLua,2);
	lua_pushusertag(pkLua, var, tolua_tag(pkLua, "BasicConsole"));
	return 1;
}

void GameScript::OpenPackageFiles()
{
	tolua_zfscript_bind_open(GetLua());

	ExposeClass("Console", LuaGetBasicConsole, LuaSetBasicConsole);
}






