// zfscript.h

#ifndef _ZFSCRIPT_H
#define _ZFSCRIPT_H

extern "C"  
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

#include <tolua++.h>

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
	tINT				= 1,
	tBOOL				= 2, 
	tFLOAT			= 3,
	tDOUBLE			= 4,
	tCSTRING			= 5,
	tSTLSTRING		= 6,
	tUSERDATA		= 7, // only for function arguments var so far
};

struct SCRIPT_API TABLE_DATA
{
	void* pData;
	bool bNumber;
};

class SCRIPT_API ScriptFuncArg
{
public:
	struct Type
	{
		ScripVarType m_eType;
		char m_szType[50];
	} m_kType;

	void* m_pData;
	
	ScriptFuncArg(void* pData, const char* szTypeName) 
	{ 
		m_pData = pData; 
		m_kType.m_eType = tUSERDATA; 
		strcpy(m_kType.m_szType, szTypeName); 
	};

	ScriptFuncArg(void* pData, ScripVarType eType)
	{ 
		m_pData = pData; 
		m_kType.m_eType = eType; 
	};

	ScriptFuncArg()
	{
		m_pData = NULL;
		m_kType.m_eType = tINT;
	}
};

/**	\brief	Script in the Resource SubSystem
		\ingroup Script
*/
class SCRIPT_API ZFScript : public ZFResource
{
	private:
		char* m_szScriptName;		
		
		friend class ZFScriptSystem;

	public:
		lua_State* m_pkLuaState;

		ZFScript();
		~ZFScript();
		bool Create(string strName);	// overloaded
		int CalculateSize();
};

SCRIPT_API ZFResource* Create__ZFScript();

/**	\brief	SubSys that handles scripts.
		\ingroup Script
*/
class SCRIPT_API ZFScriptSystem  : public ZFSubSystem
{
private:

	struct GlobalFuncInfo
	{
		lua_CFunction pkFunction;
		char* szName;
	};

	struct GlobalVarInfo
	{
		void* pvData;
		ScripVarType eType;
		char* szName;
	};

	lua_State* m_pkLua;
	ZFVFileSystem* m_pkFileSys;

	vector<GlobalFuncInfo*> m_vkGlobalFunctions;
	vector<GlobalVarInfo*> m_vkGlobalVariables;
	vector<lua_CFunction> m_vkGlobalModules;

	static map<string,pair<void*,ScripVarType> > m_kVarMap;

	void Close();
	bool Open();

	bool Run(ZFScript* pkScript);
	void CreateMetatables(lua_State* L);
	
	static int GetVar(lua_State* L);
	static int SetVar(lua_State* L);

	void CopyGlobalData(lua_State* pkState);
	
protected:
	lua_State* GetLua() { return m_pkLua; }

	friend class ZFScript;

public:

	bool Call(ZFResourceHandle* pkResHandle, const char* szFuncName, vector<ScriptFuncArg>& vkParams);
	bool Call(ZFResourceHandle* pkResHandle, const char* szFuncName, int iNumParams, int iNumResults);
	//bool Run(ZFResourceHandle* pkResHandle);

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

	bool GetArgInt(lua_State* state, int iIndex, int* data);
	bool GetArgNumber(lua_State* state, int iIndex, double* data);
	bool GetArgString(lua_State* state, int iIndex, char* data);
	bool GetArgString(lua_State* state, int iIndex, string& strData);
	bool GetArgTable(lua_State* state, int iIndex, vector<TABLE_DATA>& data);
	void DeleteTable(vector<TABLE_DATA>& data);

	void ExposeFunction(const char* szName, lua_CFunction o_Function, lua_State* pkState=NULL);
	void ExposeVariable(const char* szName, void* pVar, ScripVarType eType, lua_State* L=NULL);
	void ExposeModule(lua_CFunction o_OpenFunction, lua_State* L=NULL);
	
	string GetCallAdress(lua_State* pkLua);
	string GetFunctionName(lua_State* pkLua, int iStackPosition);
	
	bool VerifyArg(lua_State* pkLua, int iNumOfArgument);
	void Error(lua_State* pkLua, const char* szMessageFmt,...);

	bool StartUp();
	bool ShutDown();
	bool IsValid();	
	
	string FormatMultiLineTextFromLua(string strLuaText);
	
	ZFScriptSystem();
	virtual ~ZFScriptSystem();

};

#endif // #ifndef _ZFSCRIPT_H
