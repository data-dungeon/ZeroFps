// si_properties.cpp: Scriptfunctions of common used properties here. //
// Common property = one that is not unique for a single game.		  //
////////////////////////////////////////////////////////////////////////
#include "si_properties.h"
#include "../../basic/zfvfs.h"
#include "../../engine/entitymanager.h"
#include "../../engine_systems/propertys/p_controller.h"
#include "../../engine_systems/propertys/p_mad.h"

ZSSScriptSystem* PropertiesLua::g_pkScript;
ZSSEntityManager*  PropertiesLua::g_pkObjMan;


// ------------------------------------------------------------------------------------------


void PropertiesLua::Init(ZSSScriptSystem* pkScript, ZSSEntityManager* pkObjMan)
{
	g_pkScript = NULL;
	g_pkObjMan = NULL;
}






