// zfscript.h: interface for the ZFScript class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZFSCRIPT_H__8BAF0143_B0D3_476F_8410_552548EB16B2__INCLUDED_)
#define AFX_ZFSCRIPT_H__8BAF0143_B0D3_476F_8410_552548EB16B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "script_x.h"

extern "C"  {
	#include <lua.h>
	#include <lualib.h>
}

enum VarType
{
	tINT,
	tDOUBLE,
	tFLOAT,
	tSTRING,
};

typedef int (*LuaCallback) (lua_State* lua);

class SCRIPT_API ZFScript  
{
public:
	
	bool ExposeFunction(const char* szName, LuaCallback o_Function);
	bool ExposeVariable(const char* szName, void* pkData, VarType eVariableType);
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

	bool OpenLua();
	lua_State* m_pkLua;

	int m_iLuaTagInt;
	int m_iLuaTagDouble;
	int m_iLuaTagFloat;
	int m_iLuaTagString;
};

#endif // !defined(AFX_ZFSCRIPT_H__8BAF0143_B0D3_476F_8410_552548EB16B2__INCLUDED_)
