// scriptinterfaces.cpp: implementation of the ScriptInterfaces class.
//
//////////////////////////////////////////////////////////////////////

#include "../../script/zfscript.h"
#include "../audio/audiosystem.h"
#include "../audio/audiosystem.h"
#include "../../engine/objectmanager.h"
#include "si_audio.h"

ZFScriptSystem* AudioLua::g_pkScript;
ZFAudioSystem* AudioLua::g_pAudioSys;
ObjectManager* AudioLua::g_pkObjectMan;

void AudioLua::Init(ZFAudioSystem* pkAudio, ObjectManager* pkObjMan,
						  ZFScriptSystem* pkScript)
{
	g_pkScript = pkScript;
	g_pAudioSys = pkAudio;
	g_pkObjectMan = pkObjMan;

	pkScript->ExposeFunction("PlaySound",	AudioLua::PlaySoundLua);
}

// PlaySound
// Parameters:
//		(0) name of sound (not the full path) (char*)
// sedan antingen:
//		(1) objekt id (int)
//	eller
//		(1,2,3) pos x, pos y, pos z (double, double, double)
//		(4,5,6) dir x, dir y, dir z (double, double, double)
int AudioLua::PlaySoundLua(lua_State* pkLua)
{
	int iNumArgs = g_pkScript->GetNumArgs(pkLua);

	double dValue;
	Vector3 pos, dir;
	string strFileName;
	
	switch(iNumArgs)
	{
	case 2:
		g_pkScript->GetArgNumber(pkLua, 1, &dValue);
		Object* pkObject;
		pkObject = g_pkObjectMan->GetObjectByNetWorkID((int)dValue);
		pos = pkObject->GetWorldPosV();
		dir = pkObject->GetWorldRotV();
		break;

	case 7:
		g_pkScript->GetArgNumber(pkLua, 1, &dValue); pos.x = dValue;
		g_pkScript->GetArgNumber(pkLua, 2, &dValue); pos.y = dValue;
		g_pkScript->GetArgNumber(pkLua, 3, &dValue); pos.z = dValue;

		g_pkScript->GetArgNumber(pkLua, 4, &dValue); dir.x = dValue;
		g_pkScript->GetArgNumber(pkLua, 5, &dValue); dir.y = dValue;
		g_pkScript->GetArgNumber(pkLua, 6, &dValue); dir.z = dValue;
		break;

	default:
		printf("Failed to play sound! Wrong number of arguments.\n");
		return 0;
	}

	// Name of the sound file (not the full path)
	char szFileName[100];
	g_pkScript->GetArg(pkLua, 0, szFileName);

	// Create full path
	strFileName = "data/sound/";
	strFileName.append(szFileName);

	ZFSound* pkSound = g_pAudioSys->GetFreeSound(strFileName);
	pkSound->m_kPos = pos;
	pkSound->m_kVel = dir;
	pkSound->m_bLoop = false;

	g_pAudioSys->AddSound(pkSound);

	printf("Trying to play sound from script\n");

	return 1;
}
