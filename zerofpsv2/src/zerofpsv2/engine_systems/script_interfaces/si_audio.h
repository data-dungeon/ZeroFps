// scriptinterfaces.h: interface for the ScriptInterfaces class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SI_GUI_H
#define _SI_GUI_H

#include "../../script/zfscript.h"
#include "../engine_systems_x.h"

class ZSSScriptSystem;
class ZSSAudioSystem;
class ZSSEntityManager;

namespace AudioLua
{
	void ENGINE_SYSTEMS_API Init(ZSSAudioSystem* pkAudio, ZSSEntityManager* pkObjMan,
		ZSSScriptSystem* pkScript);

	extern ZSSScriptSystem* g_pkScript;
	extern ZSSAudioSystem* g_pAudioSys;
	extern ZSSEntityManager* g_pkEntityMan;

	int ENGINE_SYSTEMS_API PlaySoundLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API StopSoundLua(lua_State* pkLua);

	int ENGINE_SYSTEMS_API PlayGuiSoundLua(lua_State* pkLua);

	int ENGINE_SYSTEMS_API PlayMusicLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API StopMusicLua(lua_State* pkLua);


};

#endif // #ifndef _SI_GUI_H
