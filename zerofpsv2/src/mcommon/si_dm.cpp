#include "si_dm.h"
#include "../zerofpsv2/script/zfscript.h"

ZFScriptSystem*			DMLua::g_pkScript;
EntityManager*				DMLua::g_pkObjMan;

void DMLua::Init(EntityManager* pkObjMan,ZFScriptSystem* pkScript)
{
	g_pkObjMan = pkObjMan;
	g_pkScript = pkScript;
	
	pkScript->ExposeFunction("GetClosestDMCharacter", DMLua::GetClosestDMCharacterLua);
}

int DMLua::GetClosestDMCharacterLua(lua_State* pkLua) 
{
	g_pkScript->AddReturnValue(pkLua, "linux", strlen("linux") );

	return 1; // this function returns one (1) argument
}