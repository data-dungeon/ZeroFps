// zfscript.cpp: implementation of the ZFScriptSystem class.
//
//////////////////////////////////////////////////////////////////////
 
#include "zfscript.h"
#include <stdio.h> 
#include "../basic/zfvfs.h"
#include "../basic/zfassert.h"
#include <iostream>
   using namespace std;
  
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

	m_pkFileSys = reinterpret_cast<ZFVFileSystem*>(
		g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));	

	return true;	
}

void ZFScriptSystem::CopyGlobalData(lua_State** ppkState)
{
	*ppkState = lua_open(0);

	// Create Lua tag for Int type.
/*	m_iLuaTagInt = lua_newtag(*ppkState);
	lua_pushcfunction(*ppkState, GetTypeInt);
	lua_settagmethod(*ppkState, m_iLuaTagInt, "getglobal");
	lua_pushcfunction(*ppkState, SetTypeInt); 
	lua_settagmethod(*ppkState, m_iLuaTagInt, "setglobal");

	// Create Lua tag for Double type.
	m_iLuaTagDouble = lua_newtag(*ppkState);
	lua_pushcfunction(*ppkState, GetTypeDouble);
	lua_settagmethod(*ppkState, m_iLuaTagDouble, "getglobal");
	lua_pushcfunction(*ppkState, SetTypeDouble); 
	lua_settagmethod(*ppkState, m_iLuaTagDouble, "setglobal");

	// Create Lua tag for Float type.
	m_iLuaTagFloat = lua_newtag(*ppkState);
	lua_pushcfunction(*ppkState, GetTypeFloat);
	lua_settagmethod(*ppkState, m_iLuaTagFloat, "getglobal");
	lua_pushcfunction(*ppkState, SetTypeFloat); 
	lua_settagmethod(*ppkState, m_iLuaTagFloat, "setglobal");

	// Create Lua tag for String type.
	m_iLuaTagString = lua_newtag(*ppkState);
	lua_pushcfunction(*ppkState, GetTypeString);
	lua_settagmethod(*ppkState, m_iLuaTagString, "getglobal");
	lua_pushcfunction(*ppkState, SetTypeString); 
	lua_settagmethod(*ppkState, m_iLuaTagString, "setglobal");*/

	unsigned int i;
	unsigned int iNumFunctions = m_vkGlobalFunctions.size();
	unsigned int iNumVars = m_vkGlobalVariables.size();

	// Add global functions
	for(i=0; i<iNumFunctions;  i++)
		ExposeFunction(m_vkGlobalFunctions[i]->szName,
			m_vkGlobalFunctions[i]->pkFunction, 
			*ppkState);

	// Add global variables
	for(i=0; i<iNumVars; i++)
		ExposeVariable(m_vkGlobalVariables[i]->szName,
			m_vkGlobalVariables[i]->pvData,
			m_vkGlobalVariables[i]->eType, 
			*ppkState);
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
// Name:		RegisterClass
// Description:	Registrera en C++ klass som Lua kan se.
//
bool ZFScriptSystem::ExposeClass(char *szName, ScripObjectType eType, 
						   lua_CFunction o_LuaGet, lua_CFunction o_LuaSet)
{
	// check if type is already registered
/*	bool bAlreadyExist = true;

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
*/
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name:		ExposeClass
// Description:	Registrera en C++ klass som Lua kan se.
//
bool ZFScriptSystem::ExposeObject(const char* szName, void* pkData, ScripObjectType eType)
{
/*
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
	lua_setglobal(m_pkLua, szName);*/
	return true;
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

///////////////////////////////////////////////////////////////////////////////
// Name:		ExposeVariable
// Description:	Registrera en C++ variabel som Lua kan se.
//
bool ZFScriptSystem::ExposeVariable(const char* szName, void* pkData, ScripVarType eType,
												lua_State* pkState)
{
	if(pkState == NULL)
		pkState = m_pkLua;

	switch(eType)
	{
	case tINT:
		lua_pushusertag(pkState, pkData, m_iLuaTagInt);
		lua_setglobal(pkState, szName);
		break;
	case tDOUBLE:
		lua_pushusertag(pkState, pkData, m_iLuaTagDouble);
		lua_setglobal(pkState, szName);
		break;
	case tFLOAT:
		lua_pushusertag(pkState, pkData, m_iLuaTagFloat);
		lua_setglobal(pkState, szName);
		break;
	case tSTRING:
		lua_pushusertag(pkState, pkData, m_iLuaTagString);
		lua_setglobal(pkState, szName);
		break;
	}

	if(pkState == m_pkLua)
	{
		GlobalVarInfo* var_info = new GlobalVarInfo;
		var_info->eType = eType;
		var_info->szName = new char[ strlen(szName) + 1 ];
		strcpy(var_info->szName, szName);
		var_info->pvData = pkData;
		m_vkGlobalVariables.push_back(var_info);

		//printf("  Adding global variable (%i) : %s\n", m_vkGlobalVariables.size(), szName);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Ett gäng statiska funktioner som triggas från Lua när man behöver komma åt
// en global c++ variabel från skript (som i förväg har blivit exponerad).
//
// Int
int ZFScriptSystem::SetTypeInt(lua_State* pkLua) {
	int* var=(int*) lua_touserdata(pkLua,2);
	int  val=(int)  lua_tonumber(pkLua,3);
	*var=val;
	return 0;
}
int ZFScriptSystem::GetTypeInt(lua_State* pkLua) {
	int* var=(int*) lua_touserdata(pkLua,2);
	lua_pushnumber(pkLua,*var);
	return 1;
}
// Double
int ZFScriptSystem::SetTypeDouble(lua_State* pkLua) {
	double* var=(double*) lua_touserdata(pkLua,2);
	double  val=(double)  lua_tonumber(pkLua,3);
	*var=val;
	return 0;
}
int ZFScriptSystem::GetTypeDouble(lua_State* pkLua) {
	double* var=(double*) lua_touserdata(pkLua,2);
	lua_pushnumber(pkLua,*var);
	return 1;
}
// Float
int ZFScriptSystem::SetTypeFloat(lua_State* pkLua) {
	float* var=(float*) lua_touserdata(pkLua,2);
	float  val=(float)  lua_tonumber(pkLua,3);
	*var=val;
	return 0;
}
int ZFScriptSystem::GetTypeFloat(lua_State* pkLua) {
	float* var=(float*) lua_touserdata(pkLua,2);
	lua_pushnumber(pkLua,*var);
	return 1;
}
// C-String
int ZFScriptSystem::SetTypeString(lua_State* pkLua) {
	char* var= (char*) lua_touserdata(pkLua,2);
	char* val= (char*) lua_tostring(pkLua,3);
	var=val;
	return 0;
}
int ZFScriptSystem::GetTypeString(lua_State* pkLua) {
	char* var=(char*) lua_touserdata(pkLua,2);
	lua_pushstring(pkLua,var);
	return 1;
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
		char* text = (char*) data;
		strcpy(text, val);
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

	if(!lua_istable(state, -1))
	{
		//printf("Failed to get global %s from script!\n. Is not a table", szTable);
		return false;
	}

	lua_pushstring (state, szVar);
	lua_gettable (state, -2);
	if (!lua_isstring (state, -1)) {
		//printf("Failed to get global %s from script!\n", szTable);
		//printf("data in %s is not a string\n", szVar);
		lua_pop (state, 1);
		return false;
	}
       
	const char* d = lua_tostring(state, -1);
	strcpy(data, d);

	lua_pop (state, 1);

	return true;
}

bool ZFScriptSystem::GetGlobal(lua_State* state, char* szTable, char* szVar, double& data)
{
	if(state == NULL)
		state = m_pkLua;

	lua_getglobal(state, szTable);

	if(!lua_istable(state, -1))
	{
		//printf("Failed to get global %s from script!\n. Is not a table", szTable);
		return false;
	}

	lua_pushstring (state, szVar);
	lua_gettable (state, -2);
	if (!lua_isnumber (state, -1)) {
		//printf("Failed to get global %s from script!\n", szTable);
		//printf("data in %s is not a string\n", szVar);
		lua_pop (state, 1);
		return false;
	}
        
	data = lua_tonumber(state, -1);

	lua_pop (state, 1);

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
	m_pkLuaState = lua_open(0);
		
	if(strName.empty())
	{
		printf("Failed to create script resource, bad filename!\n");
		return false;
	}

	m_szScriptName = new char[strName.size()+1];				// LEAK - MistServer, Nothing loaded. (SKALL FÖRSTÖRAS MEN GÖRS EJ EFTERSOM EXPIRE KITTET INTE FUNKAR ÄN)
	strcpy(m_szScriptName, strName.c_str());

	ZFScriptSystem* pkScriptSys = static_cast<ZFScriptSystem*>(g_ZFObjSys.GetObjectPtr("ZFScriptSystem"));

	pkScriptSys->CopyGlobalData(&m_pkLuaState);

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

	if( lua_dofile(pkScript->m_pkLuaState, strPath.c_str()) != 0 )
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
		ZFAssert(0, szError);
//		return false; // TEST VC7!!!
	}

	if( lua_dofile(pkScript->m_pkLuaState, strPath.c_str()) != 0 )
	{
		sprintf(szError, "Failed to run script! \"%s\" does not exist.\n", strPath.c_str());
		printf(szError);
		ZFAssert(0, szError);	
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

	return (lua_call(pkScript->m_pkLuaState, vkParams.size(), 0) == 0);
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

	bool bSuccess = (lua_call(pkScript->m_pkLuaState, iNumParams, iNumResults) == 0);

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