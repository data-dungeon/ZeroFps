// zfscript.cpp: implementation of the ZFScriptSystem class.
//
//////////////////////////////////////////////////////////////////////
 
#include "zfscript.h"
#include <stdio.h> 
#include "../basic/zfvfs.h"
#include "../basic/zfassert.h"
#include <iostream>

using namespace std;

map<string,pair<void*,VAR_TYPE> > ZFScriptSystem::m_kVarMap;

  
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
  
ZFScriptSystem::ZFScriptSystem() : ZFSubSystem("ZFScriptSystem")
{
	Open(); 
}

ZFScriptSystem::~ZFScriptSystem()
{
	Close();
}


bool ZFScriptSystem::StartUp()
{ 
	

	return true; 
}

bool ZFScriptSystem::ShutDown() 
{ 
	return true; 
}

bool ZFScriptSystem::IsValid()	{ return true; }


///////////////////////////////////////////////////////////////////////////////
// Name:		OpenLua
// Description:	Starta lua, öppna vissa hjälpfunktioner och skapa ett alla de
//				tag metoder som krävs för att kunna registrera globala variabler
//				till Lua.
//
bool ZFScriptSystem::Open()
{
	// Open Lua
	m_pkLua = lua_open();


	// Create Lua tag for Int type.
	//m_iLuaTagInt = lua_newtag(m_pkLua);
	//lua_pushcfunction(m_pkLua, GetTypeInt);
	//lua_settagmethod(m_pkLua, m_iLuaTagInt, "getglobal");
	//lua_pushcfunction(m_pkLua, SetTypeInt); 
	//lua_settagmethod(m_pkLua, m_iLuaTagInt, "setglobal");

	//// Create Lua tag for Double type.
	//m_iLuaTagDouble = lua_newtag(m_pkLua);
	//lua_pushcfunction(m_pkLua, GetTypeDouble);
	//lua_settagmethod(m_pkLua, m_iLuaTagDouble, "getglobal");
	//lua_pushcfunction(m_pkLua, SetTypeDouble); 
	//lua_settagmethod(m_pkLua, m_iLuaTagDouble, "setglobal");

	//// Create Lua tag for Float type.
	//m_iLuaTagFloat = lua_newtag(m_pkLua);
	//lua_pushcfunction(m_pkLua, GetTypeFloat);
	//lua_settagmethod(m_pkLua, m_iLuaTagFloat, "getglobal");
	//lua_pushcfunction(m_pkLua, SetTypeFloat); 
	//lua_settagmethod(m_pkLua, m_iLuaTagFloat, "setglobal");

	//// Create Lua tag for String type.
	//m_iLuaTagString = lua_newtag(m_pkLua);
	//lua_pushcfunction(m_pkLua, GetTypeString);
	//lua_settagmethod(m_pkLua, m_iLuaTagString, "getglobal");
	//lua_pushcfunction(m_pkLua, SetTypeString); 
	//lua_settagmethod(m_pkLua, m_iLuaTagString, "setglobal");

	m_pkFileSys = reinterpret_cast<ZFVFileSystem*>(
		g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));	

	// Open base lib for access to some useful functions.
	lua_baselibopen(m_pkLua);
	lua_strlibopen(m_pkLua);
	lua_mathlibopen(m_pkLua);
	lua_iolibopen(m_pkLua);
	lua_dblibopen(m_pkLua);

	return true;	
}

void ZFScriptSystem::CopyGlobalData(lua_State* pkState)
{
	//*ppkState = lua_open();

	unsigned int i;
	const unsigned int iNumFunctions = m_vkGlobalFunctions.size();
	const unsigned int iNumVars = m_vkGlobalVariables.size();

	// Add global functions
	for(i=0; i<iNumFunctions;  i++)
		ExposeFunction(m_vkGlobalFunctions[i]->szName,
			m_vkGlobalFunctions[i]->pkFunction, 
			pkState);

	// Add global variables
	for(i=0; i<iNumVars; i++)
		ExposeVariable(m_vkGlobalVariables[i]->szName,
			m_vkGlobalVariables[i]->pvData,
			m_vkGlobalVariables[i]->eType,
			pkState);
}

void ZFScriptSystem::Close()
{
	lua_close(m_pkLua);

	unsigned int i;

	for(i=0; i<m_vkGlobalFunctions.size(); i++)
	{
		delete[] m_vkGlobalFunctions[i]->szName;
		delete m_vkGlobalFunctions[i];
	}

	for(i=0; i<m_vkGlobalVariables.size(); i++)
	{
		delete[] m_vkGlobalVariables[i]->szName;
		delete m_vkGlobalVariables[i];
	}

	printf("ZFScriptSystem::Close\n");
}

///////////////////////////////////////////////////////////////////////////////
// Name:		ExposeFunction
// Description:	Registrera en C++ function som Lua kan se.
//
bool ZFScriptSystem::ExposeFunction(const char *szName, lua_CFunction o_Function, 
												lua_State* pkState)
{
	if(pkState == NULL)
		pkState = m_pkLua;

	lua_register( pkState, szName, o_Function );

	if(pkState == m_pkLua)
	{
		GlobalFuncInfo* func_info = new GlobalFuncInfo;			// LEAK - MistServer, Nothing loaded.	(FIXED)	
		func_info->pkFunction = o_Function;
		func_info->szName = new char[ strlen(szName) + 1 ];	// LEAK - MistServer, Nothing loaded.	(FIXED)
		strcpy(func_info->szName, szName);
		m_vkGlobalFunctions.push_back(func_info);

		//printf(" Adding global function (%i) : %s\n", m_vkGlobalFunctions.size(), szName);
	}

	return true;
}


int ZFScriptSystem::GetNumArgs(lua_State* state)
{
	return lua_gettop(state);
}

bool ZFScriptSystem::GetArg(lua_State* state, int iIndex, void* data)
{
	int iLuaIndex = iIndex + 1;

	if(lua_isnumber(state, iLuaIndex))
	{
		double val = lua_tonumber(state, iLuaIndex);
		double* number = (double*) data;
		*number = val;
		return true;
	}

	if(lua_isstring(state, iLuaIndex))
	{
		const char* val = lua_tostring(state, iLuaIndex);
		char* text = (char*) data;
		strcpy(text, val);
		return true;
	}

	return false;
}

bool ZFScriptSystem::GetArgTable(lua_State* state, int iIndex, vector<TABLE_DATA>& vkData)
{
    // table is in the stack at index `t'
    lua_pushnil(state);  // first key
    while (lua_next(state, iIndex) != 0) 
	 {
			TABLE_DATA table_data;

			if(lua_isnumber(state, -1))
			{
				double nr = lua_tonumber(state, -1);
				table_data.bNumber = true;
				table_data.pData = new double;
				(*(double*) table_data.pData) = nr;
			}
			else
			if(lua_isstring(state, -1))
			{
				const char* str = lua_tostring(state, -1);
				table_data.bNumber = false;
				table_data.pData = new char[strlen(str)+1];
				strcpy( (char*) table_data.pData, str);
			}

			vkData.push_back(table_data);

			lua_pop(state, 1);  // removes `value'; keeps `key' for next iteration
    }

	 return true;
}

// returnerar en tabel med data till lua
void ZFScriptSystem::AddReturnValueTable(lua_State* state, vector<TABLE_DATA>& vkData)
{
	lua_newtable(state);

	for (unsigned int i=0; i<vkData.size(); i++) 
	{
		lua_pushnumber(state, i+1);

		if(vkData[i].bNumber == false)
			lua_pushstring(state, (char*) vkData[i].pData);
		else
			lua_pushnumber(state, (*(double*) vkData[i].pData));

		lua_settable(state, -3);
	}

	DeleteTable(vkData);
}

bool ZFScriptSystem::GetArgNumber(lua_State* state, int iIndex, double* data)
{
	int iLuaIndex = iIndex + 1;

	if(lua_isnumber(state, iLuaIndex))
	{
		double val = lua_tonumber(state, iLuaIndex);
		double* number = (double*) data;
		*number = val;
		return true;
	}

	if(lua_isstring(state, iLuaIndex))
	{
		const char* val = lua_tostring(state, iLuaIndex);
		char* text = (char*) data;
		strcpy(text, val);
		return true;
	}

	return false;
}

bool ZFScriptSystem::GetArgString(lua_State* state, int iIndex, char* data)
{
	int iLuaIndex = iIndex + 1;

	if(lua_isstring(state, iLuaIndex))
	{
		const char* val = lua_tostring(state, iLuaIndex);
		//char* text = (char*) data;
		
      
      strcpy(data, val);
		return true;
	}

	return false;
}

const int ZFScriptSystem::GetGlobalInt(lua_State* state, char* szName, bool* bSuccess) const
{
	if(state == NULL)
		state = m_pkLua;

	lua_getglobal(state, szName);

	int data;

	if(lua_isnumber(state, 1))
	{
		data = (int) lua_tonumber(state, 1);
		lua_pop (state, 1);

		if(bSuccess)
			bSuccess = false;

		return data;
	}

	lua_pop (state, 1);

	if(bSuccess)
		bSuccess = false;

	return -1;
}

bool ZFScriptSystem::GetGlobal(lua_State* state, char* szName, double& data)
{
	if(state == NULL)
		state = m_pkLua;

	lua_getglobal(state, szName);

	if(lua_isnumber(state, 1))
	{
		data = lua_tonumber(state, 1);
		lua_pop (state, 1);
		return true;
	}

	lua_pop (state, 1);

	return false;
}

bool ZFScriptSystem::GetGlobal(lua_State* state, char* szName, char* data)
{
	if(state == NULL)
		state = m_pkLua;

	lua_getglobal(state, szName);

	if(lua_isstring(state, 1))
	{
		strcpy(data, "\0");
		const char* d = lua_tostring(state, 1);
		if(d)
		{
			strcpy(data, d);
			lua_pop(state, 1);  // remove variable szName from the stack (VERY IMPORTENT)           
			return true;
		}
	}

	//printf("Failed to get global %s from script!\n", szName);
	lua_pop (state, 1);

	return false;
}

bool ZFScriptSystem::GetGlobal(lua_State* state, char* szTable, char* szVar, char* data)
{
	if(state == NULL)
		state = m_pkLua;

	lua_getglobal(state, szTable);

	if(lua_type(state, -1) != LUA_TTABLE)
	{
		printf("Failed to get global from script! %s Is not a table\n", szTable);
		return false;
	}

	lua_pushstring (state, szVar);
	lua_gettable (state, -2);
	if (!lua_isstring (state, -1)) {
		//printf("Failed to get global %s from script!\n", szTable);
		//printf("data in %s is not a string\n", szVar);
		lua_pop (state, 2); // 041030 - /* lua_pop (state, 1); */
		return false;
	}
       
	const char* d = lua_tostring(state, -1);
	strcpy(data, d);

	lua_pop (state, 2); // 041030 - /* lua_pop (state, 1); */

	return true;
}

bool ZFScriptSystem::GetGlobal(lua_State* state, char* szTable, char* szVar, double& data)
{
	if(state == NULL)
		state = m_pkLua;

	lua_getglobal(state, szTable);

	if(lua_type(state, -1) != LUA_TTABLE)
	{
		printf("Failed to get global from script! %s Is not a table\n", szTable);
		return false;
	}

	lua_pushstring (state, szVar);
	lua_gettable (state, -2);
	if (!lua_isnumber (state, -1)) {
		//printf("Failed to get global %s from script!\n", szTable);
		//printf("data in %s is not a string\n", szVar);
		lua_pop (state, 2); // 041030 - /* lua_pop (state, 1); */
		return false;
	}
        
	data = lua_tonumber(state, -1);

	lua_pop (state, 2); // 041030 - /* lua_pop (state, 1); */

	return true;
}

void ZFScriptSystem::AddReturnValue(lua_State* state, double dValue)
{
	if(state == NULL)
		state = m_pkLua;

	lua_pushnumber(state, dValue); 
}

void ZFScriptSystem::AddReturnValue(lua_State* state,char *szValue, int legth)
{
	if(state == NULL)
		state = m_pkLua;

	lua_pushlstring(state, szValue, legth); 
}

///////////////////////////////////////////////////////////////////////////// 
/*
	ZFScript (Resource file)

////////////////////////////////////////////////////////////////////////////*/


ZFScript::ZFScript()
{
	m_szScriptName = NULL;
	m_pkLuaState = NULL;
}

ZFScript::~ZFScript()
{
	if(m_szScriptName != NULL)
	{
		delete[] m_szScriptName;
		m_szScriptName = NULL;
	}

	if(m_pkLuaState != NULL)
	{
		lua_close(m_pkLuaState);
		m_pkLuaState = NULL;
	}
}

bool ZFScript::Create(string strName)
{
	m_pkLuaState = lua_open();
		
	if(strName.empty())
	{
		printf("Failed to create script resource, bad filename!\n");
		return false;
	}

	m_szScriptName = new char[strName.size()+1];				// LEAK - MistServer, Nothing loaded. (SKALL FÖRSTÖRAS MEN GÖRS EJ EFTERSOM EXPIRE KITTET INTE FUNKAR ÄN)
	strcpy(m_szScriptName, strName.c_str());

	ZFScriptSystem* pkScriptSys = static_cast<ZFScriptSystem*>(g_ZFObjSys.GetObjectPtr("ZFScriptSystem"));

	pkScriptSys->CreateMetatables(m_pkLuaState);
	pkScriptSys->CopyGlobalData(m_pkLuaState);

	return pkScriptSys->Run(this);
}


ZFResource* Create__ZFScript()
{
	return new ZFScript;							// LEAK - MistServer, Nothing loaded.
}

int ZFScript::CalculateSize()
{
	return 4;
}

bool ZFScriptSystem::Run(ZFResourceHandle* pkResHandle)
{	
	ZFScript* pkScript = (ZFScript*) pkResHandle->GetResourcePtr();

	if(pkScript->m_pkLuaState == NULL)
		return false;

	// Försök att hitta sökvägen via det virituella filsystemet.
	string strPath = m_pkFileSys->GetFullPath(pkScript->m_szScriptName);

	char szError[150];

	if( strPath.empty() )
	{
		sprintf(szError, "Failed to run scrip: \"%s\"! Bad path.\n", strPath.c_str());
		printf(szError);
		//ZFAssert(0, szError);
		return false;
	}

	if( luaL_loadfile(pkScript->m_pkLuaState, strPath.c_str()) != 0 )
	{
		sprintf(szError, "Failed to run scrip! \"%s\" does not exist.\n", strPath.c_str());
		printf(szError);
		//ZFAssert(0, szError);	
		return false;
	}	

	return true;
}

bool ZFScriptSystem::Run(ZFScript* pkScript)
{	
	if(pkScript->m_pkLuaState == NULL)
		return false;

	// Försök att hitta sökvägen via det virituella filsystemet.
	string strPath = m_pkFileSys->GetFullPath(pkScript->m_szScriptName);

	char szError[150];

	if( strPath.empty() )
	{
		sprintf(szError, "Failed to find script: \"%s\"\n", pkScript->m_szScriptName);
		printf(szError);
		//ZFAssert(0, szError);
		//		return false; // TEST VC7!!!
	}

	int iDoFileRes = lua_dofile(pkScript->m_pkLuaState, strPath.c_str());

	if( iDoFileRes )
	{
		switch(iDoFileRes)
		{
			case LUA_ERRRUN:		sprintf(szError, "LUA_ERRRUN");		break;
			case LUA_ERRSYNTAX:	sprintf(szError, "LUA_ERRSYNTAX");		break;
			case LUA_ERRMEM:		sprintf(szError, "LUA_ERRMEM");		break;
			case LUA_ERRERR:		sprintf(szError, "LUA_ERRERR");		break;
			case LUA_ERRFILE:		sprintf(szError, "LUA_ERRFILE");		break;
		}

		//sprintf(szError, "Failed to run script! \"%s\" does not exist.\n", strPath.c_str());
		printf(szError);
		//ZFAssert(0, szError);	
	}

	return true;
}

bool ZFScriptSystem::Call(ZFResourceHandle* pkResHandle, char* szFuncName, 
									vector<ARG_DATA>& vkParams)
{
	ZFScript *pkScript = (ZFScript*) pkResHandle->GetResourcePtr();

	if(pkScript->m_pkLuaState == NULL)
		return false;

	lua_getglobal( pkScript->m_pkLuaState, szFuncName);

	// Måste kolla så att den global funktion finns. 
	// Låser sig fett om den int gör det!
	if(lua_isnil( pkScript->m_pkLuaState, 1) )
	{
		lua_pop(pkScript->m_pkLuaState, 1);

		return false;
	}

	double d;

	for(unsigned int j=0; j<vkParams.size(); j++)
	{
		switch(vkParams[j].eType)
		{
		case tINT:
			int* i; i = (int*) vkParams[j].pData; d = *i;
			lua_pushnumber(pkScript->m_pkLuaState, d);
			break;
		case tDOUBLE:			
			lua_pushnumber(pkScript->m_pkLuaState, (*(double*) vkParams[j].pData));
			break;
		case tFLOAT:
			float* f; f = (float*) vkParams[j].pData; d = *f;
			lua_pushnumber(pkScript->m_pkLuaState, d);
			break;
		case tSTRING:
			lua_pushstring(pkScript->m_pkLuaState, (char*) vkParams[j].pData);
			break;
		}
	}

	return (lua_pcall(pkScript->m_pkLuaState, vkParams.size(), 0, 0) == 0);
}

bool ZFScriptSystem::Call(ZFResourceHandle* pkResHandle, char* szFuncName, 
								  int iNumParams, int iNumResults)
{	
	ZFScript *pkScript = (ZFScript*) pkResHandle->GetResourcePtr();

	if(pkScript->m_pkLuaState == NULL)
		return false;

	// Försök att hitta sökvägen via det virituella filsystemet.
	string strPath = m_pkFileSys->GetFullPath(pkScript->m_szScriptName);

	//printf("SCRIPT_API: Calling script function %s\n", szFuncName);
	lua_getglobal( pkScript->m_pkLuaState, szFuncName);
	
	// Måste kolla så att den global funktion finns. 
	// Låser sig fett om den int gör det!
	if(lua_isnil( pkScript->m_pkLuaState, 1) )
	{
		lua_pop(pkScript->m_pkLuaState, 1);

		//if(szFuncName)
		//	printf("Failed to find Lua function: %s\n", szFuncName);

		//printf("Failed to find Lua function\n");

		return false;
	}

	bool bSuccess = (lua_pcall(pkScript->m_pkLuaState, iNumParams, iNumResults, 0) == 0);

//	lua_pop(pkScript->m_pkLuaState, 1);

	return bSuccess;
}


// ----------------------------------------------------------------------------------------------

void ZFScriptSystem::DeleteTable(vector<TABLE_DATA>& data)
{
	for(unsigned int i=0; i<data.size(); i++)
	{
		if(data[i].pData)
			delete data[i].pData;
	}
}




string ZFScriptSystem::FormatMultiLineTextFromLua(string strLuaText)
{
	string temp;

	// Ta först bort Lua's alla radtecken.
	for(int j=0; j<strLuaText.size(); j++)
	{
		if(strLuaText[j] != '\n')
			temp.push_back(strLuaText[j]);
	}

	strLuaText = temp;
	temp.clear();

	// Lägg sedan till radtecken där ett '\n' finns.
	for(int j=0; j<strLuaText.size(); j++)
	{
		if(j!=(int)strLuaText.size()-1 && j!=0)
		{
			if(strLuaText[j] == '\\' && strLuaText[j+1] == 'n')
				temp.push_back('\n');
			else
			if(strLuaText[j] == 'n' && strLuaText[j-1] == '\\')
				continue;
			else
				temp.push_back(strLuaText[j]);
		}
		else
		{
			temp.push_back(strLuaText[j]);
		}
	}

	return temp;
}

static string g_strCurrVariable = "";

// Module index function
static int module_index_event (lua_State* L)
{
	lua_pushstring(L,".get");
	lua_rawget(L,-3);
	if (lua_istable(L,-1))
	{
		lua_pushvalue(L,2);  // key
		lua_rawget(L,-2);
		if (lua_iscfunction(L,-1))
		{
			g_strCurrVariable = lua_tostring(L,-3);
			lua_call(L,0,1);
			return 1;
		}
		else if (lua_istable(L,-1))
			return 1;
	}
	// call old index meta event
	if (lua_getmetatable(L,1))
	{
		lua_pushstring(L,"__index");
		lua_rawget(L,-2);
		lua_pushvalue(L,1);
		lua_pushvalue(L,2);
		if (lua_isfunction(L,-1))
		{
			lua_call(L,2,1);
			return 1;
		}
		else if (lua_istable(L,-1))
		{
			lua_gettable(L,-3);
			return 1;
		}
	}
	lua_pushnil(L);
	return 1;
}

// Module newindex function
static int module_newindex_event (lua_State* L)
{
	lua_pushstring(L,".set");
	lua_rawget(L,-4);
	if (lua_istable(L,-1))
	{
		lua_pushvalue(L,2);  // key
		lua_rawget(L,-2);
		if (lua_iscfunction(L,-1))
		{
			g_strCurrVariable = lua_tostring(L,-4);

			lua_pushvalue(L,1); // only to be compatible with non-static vars
			lua_pushvalue(L,3); // value
			lua_call(L,2,0);
			return 0;
		}
	}
	// call old newindex meta event
	if (lua_getmetatable(L,1) && lua_getmetatable(L,-1))
	{
		lua_pushstring(L,"__newindex");
		lua_rawget(L,-2);
		if (lua_isfunction(L,-1))
		{
		 lua_pushvalue(L,1);
		 lua_pushvalue(L,2);
		 lua_pushvalue(L,3);
			lua_call(L,3,0);
		}
	}
	lua_settop(L,3);
	lua_rawset(L,-3);
	return 0;
}



void ZFScriptSystem::CreateMetatables(lua_State* L)
{
	// Create meta funktions for get/set
	lua_pushvalue(L,LUA_GLOBALSINDEX); // push module
		lua_newtable(L);
		lua_pushstring(L,"__index");
		lua_pushcfunction(L,module_index_event);
		lua_rawset(L,-3);
		lua_pushstring(L,"__newindex");
		lua_pushcfunction(L,module_newindex_event);
		lua_rawset(L,-3);
		lua_setmetatable(L,-2);
	lua_pop(L,1);               // pop module
}


///////////////////////////////////////////////////////////////////////////////
// Name:		ExposeVariable
// Description:	Registrera en standard variabel som Lua kan se.
//
int ZFScriptSystem::ExposeVariable (const char* name, void* pVar, VAR_TYPE eType, lua_State* L)
{
	if(L == NULL)
		L = m_pkLua;

	lua_pushvalue(L,LUA_GLOBALSINDEX);

		// get func
		lua_pushstring(L,".get");
		lua_rawget(L,-2);
		if (!lua_istable(L,-1))
		{
			// create .get table, leaving it at the top 
			lua_pop(L,1);
			lua_newtable(L);
			lua_pushstring(L,".get");
			lua_pushvalue(L,-2);
			lua_rawset(L,-4);
		}
		lua_pushstring(L,name);
		lua_pushcfunction(L,GetVar);
		lua_rawset(L,-3);                  // store variable 
		lua_pop(L,1);                      // pop .get table 

		// set func 
		lua_pushstring(L,".set");
		lua_rawget(L,-2);
		if (!lua_istable(L,-1))
		{
			// create .set table, leaving it at the top 
			lua_pop(L,1);
			lua_newtable(L);
			lua_pushstring(L,".set");
			lua_pushvalue(L,-2);
			lua_rawset(L,-4);
		}
		lua_pushstring(L,name);
		lua_pushcfunction(L,SetVar);
		lua_rawset(L,-3);                  // store variable 
		lua_pop(L,1);                      // pop .set table 

	lua_pop(L,1);

	m_kVarMap[name] = pair<void*,VAR_TYPE>((void*)pVar, eType);

	GlobalVarInfo* var_info = new GlobalVarInfo;
	var_info->eType = eType;
	var_info->szName = new char[ strlen(name) + 1 ];
	strcpy(var_info->szName, name);
	var_info->pvData = pVar;
	m_vkGlobalVariables.push_back(var_info);

	return 1;
}







// get function: g_iExpIntVar
int ZFScriptSystem::GetVar(lua_State* L)
{
	pair<void*, int> t = m_kVarMap[g_strCurrVariable];

	switch(t.second)
	{
	case VAR_INT:
		lua_pushnumber(L,(double)(*(int*) t.first));
		break;
	case VAR_BOOL:
		lua_pushnumber(L,(double)(*(bool*) t.first));
		break;
	case VAR_FLOAT:
		lua_pushnumber(L,(double)(*(float*) t.first));
		break;
	case VAR_DOUBLE:
		lua_pushnumber(L,(double)(*(double*) t.first));
		break;
	case VAR_CSTRING:
		lua_pushstring(L,(char*) t.first);
		break;
	case VAR_STLSTRING:
		lua_pushstring(L,(char*)(*(string*)t.first).c_str());
		break;
	}

	return 1;
}

// set function: g_iExpIntVar
int ZFScriptSystem::SetVar(lua_State* L)
{
	pair<void*, int> t = m_kVarMap[g_strCurrVariable];

	switch(t.second)
	{
	case VAR_INT:
		(*(int*) t.first) = (int) (lua_gettop(L)<abs(2) ? 0 : lua_tonumber(L,2));
		break;
	case VAR_BOOL:
		(*(bool*) t.first) = (bool) (lua_gettop(L)<abs(2) ? 0 : lua_tonumber(L,2));
		break;
	case VAR_FLOAT:
		(*(float*) t.first) = (float) (lua_gettop(L)<abs(2) ? 0 : lua_tonumber(L,2));
		break;
	case VAR_DOUBLE:
		(*(double*) t.first) = (double) (lua_gettop(L)<abs(2) ? 0 : lua_tonumber(L,2));
		break;
	case VAR_CSTRING:
		strcpy((char*) t.first, (lua_gettop(L)<abs(2) ? "" : lua_tostring(L,2)));
		break;
	case VAR_STLSTRING:
		(*(string*) t.first) = (lua_gettop(L)<abs(2) ? "" : lua_tostring(L,2));
		break;
	}

	return 0;
}