// si_properties.cpp: Scriptfunctions of common used properties here. //
// Common property = one that is not used specificly by a game.		  //
////////////////////////////////////////////////////////////////////////
#include "si_properties.h"
#include "../../basic/zfvfs.h"
#include "../../engine/entitymanager.h"

ZFScriptSystem* PropertiesLua::g_pkScript;
ZFVFileSystem*	PropertiesLua::g_pkVFS;

extern EntityManager* g_pkObjMan;

// ------------------------------------------------------------------------------------------

namespace PropertiesLua
{


void Init(ZFScriptSystem* pkScript, ZFVFileSystem* pkVFS)
{
	g_pkScript = pkScript;
	g_pkVFS = pkVFS;
	
	pkScript->ExposeFunction("BindKey",		PropertiesLua::BindKeyLua);
}

// ------------------------------------------------------------------------------------------

// objectID, NameOfKey(string), dimension (X,Y,Z), force(float), input (optional, default=KEYBOARD)
int BindKeyLua(lua_State* pkLua)
{
	string kKeyName;
	int iDim, iInput;
	float fForce;

	if( g_pkScript->GetNumArgs(pkLua) > 3 )
	{

	}

	return 0;
}
// ------------------------------------------------------------------------------------------


}
