// scriptinterfaces.cpp: implementation of the ScriptInterfaces class.
//
//////////////////////////////////////////////////////////////////////

#include "../audio/zfaudiosystem.h"
#include "../../engine/entitymanager.h"
#include "../../engine_systems/propertys/p_ambientsound.h"
#include "si_audio.h"

ZFScriptSystem* AudioLua::g_pkScript;
ZFAudioSystem* AudioLua::g_pAudioSys;
EntityManager* AudioLua::g_pkObjectMan;

void AudioLua::Init(ZFAudioSystem* pkAudio, EntityManager* pkObjMan,
						  ZFScriptSystem* pkScript)
{
	g_pkScript = pkScript;
	g_pAudioSys = pkAudio;
	g_pkObjectMan = pkObjMan;

	pkScript->ExposeFunction("PlaySound",	AudioLua::PlaySoundLua);
	pkScript->ExposeFunction("PlayGuiSound", AudioLua::PlayGuiSoundLua); 
	pkScript->ExposeFunction("StopSound",	AudioLua::StopSoundLua);
}

// PlaySound
// Parameters:
//		(0) name of sound (not the full path) (char*)
// sedan antingen:
//		(1) objekt id (int)
//		(2) loop = 1, do not loop = 0 (int)
//	eller
//		(1,2,3) pos x, pos y, pos z (double, double, double)
//		(4,5,6) dir x, dir y, dir z (double, double, double)
//		(7) loop = 1, do not loop = 0 (int)
/*int AudioLua::PlaySoundLua(lua_State* pkLua)
{
	int iNumArgs = g_pkScript->GetNumArgs(pkLua);

	double dValue;
	Vector3 pos, dir;
	string strFileName;
	bool bLoop;
	
	switch(iNumArgs)
	{
	case 3:
		g_pkScript->GetArgNumber(pkLua, 1, &dValue);
		Entity* pkObject;
		pkObject = g_pkObjectMan->GetObjectByNetWorkID((int)dValue);
		if(pkObject == NULL)
		{
			char szFileName[100];
			g_pkScript->GetArg(pkLua, 0, szFileName);
			printf("Failed to play sound \"%s\" on object with id: %i\n", 
				szFileName, (int)dValue);
			return 0;
		}
		pos = pkObject->GetWorldPosV();
		dir = pkObject->GetWorldRotV();

		g_pkScript->GetArgNumber(pkLua, 2, &dValue);
		bLoop = (dValue < 1) ? false : true;
		break;

	case 8:
		g_pkScript->GetArgNumber(pkLua, 1, &dValue); pos.x = dValue;
		g_pkScript->GetArgNumber(pkLua, 2, &dValue); pos.y = dValue;
		g_pkScript->GetArgNumber(pkLua, 3, &dValue); pos.z = dValue;

		g_pkScript->GetArgNumber(pkLua, 4, &dValue); dir.x = dValue;
		g_pkScript->GetArgNumber(pkLua, 5, &dValue); dir.y = dValue;
		g_pkScript->GetArgNumber(pkLua, 6, &dValue); dir.z = dValue;

		g_pkScript->GetArgNumber(pkLua, 7, &dValue);
		bLoop = (dValue < 1) ? false : true;
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

	g_pAudioSys->StartSound(strFileName, pos, dir, bLoop);

	return 1;
}*/

// 1:st arg Object ID generating sound
// 2:st arg name of file
int AudioLua::PlaySoundLua(lua_State* pkLua)
{
	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
	int iObjectID = (int)dTemp;

	char acName[100];
	g_pkScript->GetArg(pkLua, 1, acName);

	//printf("Play Sound '%s' on object %d\n", acName,  iObjectID);

	Entity* pkObject = g_pkObjectMan->GetObjectByNetWorkID(iObjectID);
	P_Sound* pkSound = dynamic_cast<P_Sound*>(pkObject->GetProperty("P_Sound"));

	if(pkSound)
	{
		pkSound->Play(string("data/sound/") + string(acName));
	}
	else
	{
		printf("Object have no P_Sound property! Failed to play sound.\n");
	}

	return 1;
}

// PlaySound
// Parameters:
//		(0) name of sound (not the full path) (char*)
//		(1) loop = 1, do not loop = 0 (int)
int AudioLua::PlayGuiSoundLua(lua_State* pkLua)
{
	Vector3 pos = g_pAudioSys->GetListnerPos();
	Vector3 dir = g_pAudioSys->GetListnerDir();

	int iNumArgs = g_pkScript->GetNumArgs(pkLua);

	if(iNumArgs != 2)
	{
		printf("Failed to play sound! Wrong number of arguments.\n");
		return 0;
	}

	// Name of the sound file (not the full path)
	char szFileName[100];
	g_pkScript->GetArg(pkLua, 0, szFileName);

	double dValue;
	g_pkScript->GetArgNumber(pkLua, 1, &dValue);

	bool bLoop = (dValue < 1) ? false : true;

	// Create full path
	string strFileName = "data/sound/";
	strFileName.append(szFileName);

	g_pAudioSys->StartSound(strFileName, pos, dir, false);

	return 1;
}

// StopSoundLua
// Parameters:
//		(0) name of sound (not the full path) (char*)
// sedan antingen:
//		(1) objekt id (int)
//    (2) max search range (float)
//	eller
//		(1,2,3) pos x, pos y, pos z (double, double, double)
//    (4) max search range (float)
int AudioLua::StopSoundLua(lua_State* pkLua)
{
	int iNumArgs = g_pkScript->GetNumArgs(pkLua);

	double dValue;
	Vector3 pos;
	string strFileName;
	
	float fMaxSearchRange;
	
	switch(iNumArgs)
	{
	case 3:
		g_pkScript->GetArgNumber(pkLua, 1, &dValue);
		Entity* pkObject;
		pkObject = g_pkObjectMan->GetObjectByNetWorkID((int)dValue);
		if(pkObject == NULL)
		{
			char szFileName[100];
			g_pkScript->GetArg(pkLua, 0, szFileName);
			printf("Failed to play sound \"%s\" on object with id: %i\n", 
				szFileName, (int)dValue);
			return 0;
		}
		pos = pkObject->GetWorldPosV();

		g_pkScript->GetArgNumber(pkLua, 2, &dValue);
		fMaxSearchRange = (float) dValue;
		break;

	case 5:
		g_pkScript->GetArgNumber(pkLua, 1, &dValue); pos.x = dValue;
		g_pkScript->GetArgNumber(pkLua, 2, &dValue); pos.y = dValue;
		g_pkScript->GetArgNumber(pkLua, 3, &dValue); pos.z = dValue;

		g_pkScript->GetArgNumber(pkLua, 4, &dValue);
		fMaxSearchRange = (float) dValue;
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

/*	ZFSoundInfo sound(strFileName.c_str(), pos, Vector3(0,0,1), false);
	if(!g_pAudioSys->RemoveSound(sound, fMaxSearchRange))
	{
		printf("Failed to stop sound!\n");
	}*/

	return 1;
}