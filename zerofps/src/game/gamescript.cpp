// gamescript.cpp: implementation of the GameScript class.
//
//////////////////////////////////////////////////////////////////////

#include "gamescript.h"

// package files
#include "../zerofps/engine/engine.pkg"
#include "gamescriptinterface.h"
#include "../zerofps/script/package_files/zfscript_bind.h"

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

static int LuaGetGameScriptInterface(lua_State* pkLua) 
{
	GameScriptInterface* var=(GameScriptInterface*) lua_touserdata(pkLua,2);
	lua_pushusertag(pkLua, var, tolua_tag(pkLua, "GameScriptInterface"));
	return 1;
}

static int LuaSetGameScriptInterface(lua_State* pkLua) 
{
	GameScriptInterface* var=(GameScriptInterface*) lua_touserdata(pkLua,2);
	GameScriptInterface* val=(GameScriptInterface*) lua_touserdata(pkLua,3);
	var=val;
	return 0;
}

void GameScript::OpenPackageFiles()
{	
	tolua_zfscript_bind_open(GetLua());

	ExposeClass("Console", tConsole, LuaGetBasicConsole, LuaSetBasicConsole);
	ExposeClass("Vector3", tVector3, LuaGetVector3, LuaSetVector3);
	ExposeClass("GameScriptInterface", tGame, LuaGetGameScriptInterface, LuaSetGameScriptInterface);

	// Exponera en läsa fil funktion till lua.
	ExposeFunction("GetTextFromFile", GetTextFromFile);
}

// Name: GetLogText
// Description: Open file textfile szFileName and find the text
//				between the bracet [szKey-s] and [szKey-e]
//				and copy that text (and allocate memory for) pszText.
//				DON'T FORGET TO DEALLOCATE pszText!!!
bool GameScript::GetLogText(const char *szFileName, const char *szKey, 
							char **pszText, char **pszIcon)
{
	if(szFileName == NULL || szKey == NULL)
	{
		printf("SCRIPT_API: GetTextFromScript() failed!, Bad arguments.\n");
		return false;
	}

	char szCommando[512];
	sprintf(szCommando, "LogText = GetTextFromFile(\"%s\", \"%s\")", szFileName, szKey);
	lua_dostring(GetLua(), szCommando);
	lua_getglobal(GetLua(), "LogText");

	int index = lua_gettop (GetLua());
	if(lua_isstring(GetLua(),index))
	{
		const char* szString = lua_tostring(GetLua(),index);

		if(szString == NULL || strlen(szString) < 1)
		{
			printf("SCRIPT_API: Failed to find variable %s in script file %s\n", 
				szKey, szFileName);
			return false;
		}

		int iIconLineLength = 0;
		if(szString[0] == '#')
			while(szString[1+iIconLineLength++] != '\n') {}
		
		int length = strlen(szString)-iIconLineLength;
		*pszText = new char[length+1];
		strncpy(*pszText, szString+iIconLineLength, length);

		// terminate
		if(length < strlen(szString+iIconLineLength))
			(*pszText)[length] = '\0';

		if(iIconLineLength > 0)
		{
			int offset_to_collon = 0;
			while(szString[offset_to_collon++] != '=') {}

			int length = iIconLineLength-offset_to_collon;

			*pszIcon = new char[length+1];
			strncpy(*pszIcon, szString+offset_to_collon, length);

			// terminate
			if(length < strlen(szString))
				(*pszIcon)[length] = '\0';
		}
		else
		{
			*pszIcon = new char[strlen("notex.bmp")+1];
			strcpy(*pszIcon, "notex.bmp");
		}

		return true;
	}

	return false;
}

int GameScript::GetTextFromFile(lua_State* pkLua)
{
	const char* szFileName;
	const char* szKeyName;

	int index = lua_gettop (pkLua);

	if(lua_isstring(pkLua,index-1))
		szFileName = lua_tostring(pkLua,index-1);
	else
	{
		printf("SCRIPT_API: ReadFile() failed!, Bad arguments.\n");
		return -1;
	}

	if(lua_isstring(pkLua,index))
		szKeyName = lua_tostring(pkLua,index);
	else
	{
		printf("SCRIPT_API: ReadFile() failed!, Bad arguments.\n");
		return -1;
	}

	FILE* pkFile = fopen(szFileName, "r+t");
	if(pkFile == NULL)
		return -1;

	int iLegthAll = 0;
	while(1)
	{
		int ret = getc(pkFile);
		if(ret == EOF)
			break;
		else
			iLegthAll++;
	}

	fseek(pkFile, 0, SEEK_SET);

	char* szFullText = new char[iLegthAll+1];
	fread(szFullText, sizeof(char), iLegthAll, pkFile);
	fclose(pkFile);

	char szStartKey[100], szEndKey[100];
	sprintf(szStartKey, "[%s-s]", szKeyName);
	sprintf(szEndKey, "[%s-e]", szKeyName);

	char* szTextStart = strstr(szFullText, szStartKey);
	char* szTextEnd   = strstr(szFullText, szEndKey);

	if(szTextStart == NULL || szTextEnd == NULL)
	{
		printf("Failed to find start or end key!\n");
		delete[] szFullText;
		return 1;
	}

	int start_offset = szTextStart - szFullText + 1 + strlen(szKeyName) + 4;
	int end_offset   = szTextEnd - szFullText;
	int serch_length = end_offset-start_offset;

	char* dest = szFullText+start_offset;
	char* szSearchText = new char[serch_length+1];
	strncpy(szSearchText, dest, serch_length);

	// Add null character if must...
	if(serch_length < strlen(dest))
		szSearchText[serch_length] = 0;

	delete[] szFullText;

	lua_pushlstring(pkLua, szSearchText, serch_length);

	return 1;
}


