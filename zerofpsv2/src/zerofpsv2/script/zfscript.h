// zfscript.h

#ifndef _ZFSCRIPT_H
#define _ZFSCRIPT_H

extern "C"  
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
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
	tINT				= 1,
	tDOUBLE			= 4,
	tFLOAT			= 3,
	tSTRING			= 5,
};

enum VAR_TYPE
{
	VAR_INT			= 1,
	VAR_BOOL			= 2,
	VAR_FLOAT		= 3,
	VAR_DOUBLE		= 4,
	VAR_CSTRING		= 5,
	VAR_STLSTRING	= 6
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

	bool ExposeFunction(const char* szName, lua_CFunction o_Function, lua_State* pkState=NULL);
	int ExposeVariable(const char* name, void* pVar, VAR_TYPE eType, lua_State* L=NULL);
	
	bool StartUp();
	bool ShutDown();
	bool IsValid();	

	void CopyGlobalData(lua_State* pkState);
	
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
		VAR_TYPE eType;
		char* szName;
	};

	vector<GlobalVarInfo*> m_vkGlobalVariables;

	void Close();
	bool Open();

	void CreateMetatables(lua_State* L);
	
	static int GetVar(lua_State* L);
	static int SetVar(lua_State* L);
	static map<string,pair<void*,VAR_TYPE> > m_kVarMap;

	lua_State* m_pkLua;

	ZFVFileSystem* m_pkFileSys;

protected:
	lua_State* GetLua() { return m_pkLua; }

	friend class ZFScript;

};

#endif // #ifndef _ZFSCRIPT_H
