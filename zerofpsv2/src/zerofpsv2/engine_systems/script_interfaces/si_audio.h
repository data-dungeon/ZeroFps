// scriptinterfaces.h: interface for the ScriptInterfaces class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SI_GUI_H
#define _SI_GUI_H

#include "../../script/zfscript.h"
#include "../engine_systems_x.h"

class ZFScriptSystem;
class ZFAudioSystem;
class ObjectManager;

namespace AudioLua
{
	void ENGINE_SYSTEMS_API Init(ZFAudioSystem* pkAudio, ObjectManager* pkObjMan,
		ZFScriptSystem* pkScript);

	extern ZFScriptSystem* g_pkScript;
	extern ZFAudioSystem* g_pAudioSys;
	extern ObjectManager* g_pkObjectMan;

	int ENGINE_SYSTEMS_API PlaySoundLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API PlayGuiSoundLua(lua_State* pkLua);

	int ENGINE_SYSTEMS_API StopSoundLua(lua_State* pkLua);
};

#endif // #ifndef _SI_GUI_H
