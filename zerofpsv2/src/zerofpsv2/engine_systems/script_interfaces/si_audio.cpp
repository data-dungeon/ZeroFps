// scriptinterfaces.cpp: implementation of the ScriptInterfaces class.
//
//////////////////////////////////////////////////////////////////////

#include "../audio/zfaudiosystem.h"
#include "../../engine/entitymanager.h"
#include "../../engine_systems/propertys/p_sound.h"
#include "si_audio.h"

ZFScriptSystem* AudioLua::g_pkScript;
ZFAudioSystem* AudioLua::g_pAudioSys;
EntityManager* AudioLua::g_pkEntityMan;

void AudioLua::Init(ZFAudioSystem* pkAudio, EntityManager* pkObjMan,
						  ZFScriptSystem* pkScript)
{
	cout << "Add SI: Audio" << endl;

	g_pkScript = pkScript;
	g_pAudioSys = pkAudio;
	g_pkEntityMan = pkObjMan;

	pkScript->ExposeFunction("PlaySound",	AudioLua::PlaySoundLua);
	pkScript->ExposeFunction("PlayGuiSound", AudioLua::PlayGuiSoundLua); 
	pkScript->ExposeFunction("StopSound",	AudioLua::StopSoundLua);

/*	pkScript->ExposeFunction("PlayMusic",	AudioLua::PlayMusicLua);
	pkScript->ExposeFunction("StopMusic",	AudioLua::StopMusicLua);*/
}

// PlaySound
// Parameters:
// 	1:st arg - Object ID generating sound
// 	2:nd arg - Name of file ("data/sound/plupp.wav")
// 	3:rd arg - Loop or not (0: false, 1:true)
int AudioLua::PlaySoundLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) != 3)
		return 0;

	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
	int iObjectID = (int)dTemp;

	char acName[100];
	g_pkScript->GetArg(pkLua, 1, acName);

	g_pkScript->GetArgNumber(pkLua, 2, &dTemp);		
	int iLoop = (int)dTemp;

	Entity* pkObject = g_pkEntityMan->GetEntityByID(iObjectID);
	P_Sound* pkSound = static_cast<P_Sound*>(pkObject->GetProperty("P_Sound"));

	if(pkSound)
	{
		pkSound->StartSound(string(acName), (iLoop == 0) ? false : true);
	}
	else
	{
		printf("Object have no P_Sound property! Failed to play sound.\n");
	}

	return 1;
}

// StopSound
// Parameters:
// 	1:st arg - Object ID generating sound
// 	2:nd arg - Name of file ("data/sound/plupp.wav")
int AudioLua::StopSoundLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) != 2)
		return 0;

	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
	int iObjectID = (int)dTemp;

	char acName[100];
	g_pkScript->GetArg(pkLua, 1, acName);

	Entity* pkObject = g_pkEntityMan->GetEntityByID(iObjectID);
	P_Sound* pkSound = static_cast<P_Sound*>(pkObject->GetProperty("P_Sound"));

	if(pkSound)
	{
		pkSound->StopSound(string(acName));
	}
	else
	{
		printf("Object have no P_Sound property! Failed to stop sound.\n");
	}

	return 1;
}

// PlayGuiSound
// Parameters:
//		(0) name of sound (not the full path) (char*)
int AudioLua::PlayGuiSoundLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) != 1)
	{
		printf("Failed to play sound! Wrong number of arguments.\n");
		return 0;
	}

	// Name of the sound file (not the full path)
	char szFileName[100];
	g_pkScript->GetArg(pkLua, 0, szFileName);

	g_pAudioSys->PlayAudio(string(szFileName));

	return 1;
}

int AudioLua::PlayMusicLua(lua_State* pkLua)
{
	return 1;
}

int AudioLua::StopMusicLua(lua_State* pkLua)
{
	return 1;
}







