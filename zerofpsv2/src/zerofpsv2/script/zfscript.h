// zfscript.h

#ifndef _ZFSCRIPT_H
#define _ZFSCRIPT_H

extern "C"  {
	#include <lua.h>
//	#include <lualib.h>
}

//#include <tolua.h>

#pragma warning (disable :4786)
#include <set>
#include <map>
#include <string>
using namespace std;

#include "script_x.h"
#include "../basic/zfobjectmanger.h"

#pragma warning (disable : 4251)

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

class SCRIPT_API ZFScript  : public ZFSubSystem
{
public:
	void AddReturnValue(lua_State* state,char *szValue, int legth);
	void AddReturnValue(lua_State* state, double dValue);

	const int GetGlobalInt(lua_State* state, char* szName, bool* bSuccess) const;
	bool GetGlobal(lua_State* state, char* szName, double& data);
	bool GetGlobal(lua_State* state, char* szTable, char* szVar, char* data);
	bool GetGlobal(lua_State* state, char* szTable, char* szVar, double& data);
	bool GetGlobal(lua_State* state, char* szName, char* data);

	int GetNumArgs(lua_State* state);
	bool GetArg(lua_State* state, int iNumber, void* data);

	bool GetArgNumber(lua_State* state, int iIndex, double* data);
	bool GetArgString(lua_State* state, int iIndex, char* data);

	bool ExposeClass(char *szName, ScripObjectType eType, 
	lua_CFunction o_LuaGet, lua_CFunction o_LuaSet);
	bool ExposeObject(const char* szName, void* pkData, ScripObjectType eType);
	bool ExposeVariable(const char* szName, void* pkData, ScripVarType eType);
	bool ExposeFunction(const char* szName, lua_CFunction o_Function);
	bool CallScript(char* szFuncName, int iNumParams, int iNumResults);
	bool RunScript(char* szFileName);
	
	
	bool StartUp();
	bool ShutDown();
	bool IsValid();	
	
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

#endif // #ifndef _ZFSCRIPT_H
