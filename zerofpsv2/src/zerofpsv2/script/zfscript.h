// zfscript.h

#ifndef _ZFSCRIPT_H
#define _ZFSCRIPT_H

extern "C"  
{
	#include <lua.h>
//	#include <lualib.h>
}

#pragma warning( disable : 4512) // 'class' : assignment operator could not be generated 
#pragma warning (disable : 4786)
#include <set>
#include <map>
#include <string>
using namespace std;

#include "script_x.h"
#include "../basic/zfsystem.h"
#include "../basic/zfresource.h"

class ZFVFileSystem;

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

struct SCRIPT_API TABLE_DATA
{
	void* pData;
	bool bNumber;
};

struct SCRIPT_API ARG_DATA
{
	void* pData;
	ScripVarType eType;
};

/**	\brief	Script in the Resource SubSystem
		\ingroup Script
*/
class SCRIPT_API ZFScript : public ZFResource
{
	public:
		ZFScript();
		~ZFScript();
		bool Create(string strName);	// overloaded

		int CalculateSize();
		lua_State* m_pkLuaState;

	private:

		char* m_szScriptName;
		
		friend class ZFScriptSystem;
};

SCRIPT_API ZFResource* Create__ZFScript();

/**	\brief	SubSys that handles scripts.
		\ingroup Script
*/
class SCRIPT_API ZFScriptSystem  : public ZFSubSystem
{
public:

	bool Call(ZFResourceHandle* pkResHandle, char* szFuncName, vector<ARG_DATA>& vkParams);
	bool Call(ZFResourceHandle* pkResHandle, char* szFuncName, int iNumParams, int iNumResults);
	bool Run(ZFResourceHandle* pkResHandle);

	void AddReturnValueTable(lua_State* state, vector<TABLE_DATA>& vkData);
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
	bool GetArgTable(lua_State* state, int iIndex, vector<TABLE_DATA>& data);
	void DeleteTable(vector<TABLE_DATA>& data);

	bool ExposeClass(char *szName, ScripObjectType eType, 
	lua_CFunction o_LuaGet, lua_CFunction o_LuaSet);
	bool ExposeObject(const char* szName, void* pkData, ScripObjectType eType);
	bool ExposeVariable(const char* szName, void* pkData, ScripVarType eType, lua_State* pkState=NULL);
	bool ExposeFunction(const char* szName, lua_CFunction o_Function, lua_State* pkState=NULL);
	
	bool StartUp();
	bool ShutDown();
	bool IsValid();	

	void CopyGlobalData(lua_State** ppkState);
	
	string FormatMultiLineTextFromLua(string strLuaText);
	
	ZFScriptSystem();
	virtual ~ZFScriptSystem();

private:

	bool Run(ZFScript* pkScript);

	struct GlobalFuncInfo
	{
		lua_CFunction pkFunction;
		char* szName;
	};

	vector<GlobalFuncInfo*> m_vkGlobalFunctions;

	struct GlobalVarInfo
	{
		void* pvData;
		ScripVarType eType;
		char* szName;
	};

	vector<GlobalVarInfo*> m_vkGlobalVariables;

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

	ZFVFileSystem* m_pkFileSys;

protected:
	lua_State* GetLua() { return m_pkLua; }

	friend class ZFScript;

};

#endif // #ifndef _ZFSCRIPT_H
