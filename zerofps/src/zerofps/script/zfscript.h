// zfscript.h: interface for the ZFScript class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZFSCRIPT_H__8BAF0143_B0D3_476F_8410_552548EB16B2__INCLUDED_)
#define AFX_ZFSCRIPT_H__8BAF0143_B0D3_476F_8410_552548EB16B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

extern "C"  {
	#include <lua.h>
//	#include <lualib.h>
}

#include <tolua.h>

#pragma warning (disable :4786)
#include <set>
#include <map>
#include <string>
using namespace std;

#include "script_x.h"

enum ScripVarType
{
	tINT,
	tDOUBLE,
	tFLOAT,
	tSTRING,
};

enum ScripObjectType
{
	tGame,
	tConsole,
	tVector3,
};

class SCRIPT_API ZFScript  
{
public:
	bool ExposeClass(char *szName, ScripObjectType eType, 
		lua_CFunction o_LuaGet, lua_CFunction o_LuaSet);
	bool ExposeObject(const char* szName, void* pkData, ScripObjectType eType);
	bool ExposeVariable(const char* szName, void* pkData, ScripVarType eType);
	bool ExposeFunction(const char* szName, lua_CFunction o_Function);
	bool RunScript(char* szFileName);
	ZFScript();
	virtual ~ZFScript();

private:

	static int SetTypeInt(lua_State* pkLua);
	static int GetTypeInt(lua_State* pkLua);
	static int SetTypeDouble(lua_State* pkLua);
	static int GetTypeDouble(lua_State* pkLua);
	static int SetTypeFloat(lua_State* pkLua);
	static int GetTypeFloat(lua_State* pkLua);
	static int SetTypeString(lua_State* pkLua);
	static int GetTypeString(lua_State* pkLua);

	void Close();
	bool Open();
	
	lua_State* m_pkLua;

	int m_iLuaTagInt;
	int m_iLuaTagDouble;
	int m_iLuaTagFloat;
	int m_iLuaTagString;

	set<string> m_kExposedClasses;
	map<ScripObjectType, string> m_kClassMap;

protected:
	lua_State* GetLua() { return m_pkLua; }

};

#endif // !defined(AFX_ZFSCRIPT_H__8BAF0143_B0D3_476F_8410_552548EB16B2__INCLUDED_)
