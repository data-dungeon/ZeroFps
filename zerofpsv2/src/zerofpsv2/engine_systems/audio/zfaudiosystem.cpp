//// zfaudiosystem.cpp: implementation of the ZFAudioSystem class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning( disable : 4512) // 'class' : assignment operator could not be generated 

#include <AL/al.h>
#include "AL/alc.h"
#include <AL/alut.h>
#include "../../basic/zfvfs.h"
#include "../script_interfaces/si_audio.h"
#include "../../engine/entitymanager.h"
#include "../../engine/zerofps.h"
#include "zfaudiosystem.h"

#include "SDL/SDL.h"

#define NO_SOURCE 999

Vector3 ZFAudioSystem::m_kPos = Vector3(0,0,0);

long g_iIDCounter = 0;
long g_iAmbientAreaIDCounter = 0;
				        
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//																									  //
// ZFSoundRes																					  //
//																									  //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
  
ZFSoundRes::ZFSoundRes()
{
	m_uiBufferIndexName = 0;
	m_szFileName = NULL;
	m_uiSize = 0;
}

ZFSoundRes::~ZFSoundRes()
{
	if(m_szFileName != NULL)
	{
		delete[] m_szFileName;
		m_szFileName = NULL;
	}

	// Låt OpenAL förstöra bufferten
	if(m_uiBufferIndexName != 0)
	{
		alGetError(); // clear
		alDeleteBuffers(1, &m_uiBufferIndexName);

		ALenum error;
		if( (error = alGetError()) != AL_NO_ERROR)
			ZFAudioSystem::PrintError(error, "alDeleteBuffers Failed!"); 
	}
}


///////////////////////////////////////////////////////////////////////////////
// Överlagrad funktion som anropas från resursystemet via SetRes när ett nytt
// ljud skall laddas in från disk.
///////////////////////////////////////////////////////////////////////////////
bool ZFSoundRes::Create(string strName)
{
	if(strName.empty())
		return false;

	//setup data path
// 	if(strName.find("data/audio/") == -1)
		strName = "data/audio/"+strName;
// 	else
// 		cout<<"Warning: old resource path:"<<strName<<endl;


	// Använd det virituella filsytemet för att kolla om filen finns och
	// spara undan sökvägen till filen i en medlems variabel
	ZFVFileSystem* pkFileSys;
	pkFileSys = static_cast<ZFVFileSystem*>(g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));		
	string strFull = pkFileSys->GetFullPath(strName.c_str());

	if(strFull.empty())
		return false;

	m_szFileName = new char[strFull.size()+1];
	strcpy(m_szFileName, strFull.c_str());
	
	return Load();
}


///////////////////////////////////////////////////////////////////////////////
// Ladda in en wav fil från disk mha Alut.
///////////////////////////////////////////////////////////////////////////////
bool ZFSoundRes::Load()
{
	if(m_szFileName == NULL)
		return false;

	ALsizei size=0, freq;
    ALenum format;
    ALvoid *data;
	ALboolean loop;   //we dont use this

	alGetError(); // Clear Error Code

	// Try to get a new buffer index name
	alGenBuffers(1, &m_uiBufferIndexName);

	if( alGetError() != AL_NO_ERROR)
	{
		printf("ZFSoundRes::Load(), Failed to get new buffer!\n");
		return 0;
	}

	// Load file
    alutLoadWAVFile((ALbyte*)m_szFileName, &format, &data, &size, &freq, &loop);   
	if(size==0)
	{
	   cout<<"cold not load file " << m_szFileName << ", Loading Dummy" << endl;	
	   alutLoadWAVFile((ALbyte*)DUMMY_SOUND,&format,&data, &size, &freq,&loop);
    }

	// Fill buffer
	alBufferData(m_uiBufferIndexName, format, data, size, freq);
	if( alGetError() != AL_NO_ERROR)
	{
		printf("ZFSound::Load() : alBufferData Failed!\n\n\n");
		alDeleteBuffers(1, &m_uiBufferIndexName);
		return false;
	}

	m_uiSize = size;

	//release the data
	alutUnloadWAV(format, data, size, freq);

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Undersök storleken på resursen.
///////////////////////////////////////////////////////////////////////////////
int ZFSoundRes::CalculateSize()
{
	return (int) m_uiSize;
}


///////////////////////////////////////////////////////////////////////////////
// Hämta OpenAL index för ljudbufferten. Annvänds internt av ljudsystemet.
///////////////////////////////////////////////////////////////////////////////
ALuint ZFSoundRes::GetBufferIndexName()
{
	return m_uiBufferIndexName;
}


///////////////////////////////////////////////////////////////////////////////
// Skapa ett wav ljud. Annvänds inte.
///////////////////////////////////////////////////////////////////////////////
ZFResource* Create__WavSound()
{
	return new ZFSoundRes;
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//																									  //
// ZFSoundInfo																					  //
//																									  //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ZFSoundInfo::ZFSoundInfo()
{
	ZFSoundInfo(NULL, Vector3(0,0,0), Vector3(0,0,1), false);
}

ZFSoundInfo::ZFSoundInfo(const char* c_szFile, Vector3 pos, 
						 Vector3 dir, bool bLoop)
{
	if(c_szFile != NULL)
		strcpy(m_acFile, c_szFile);

	m_bLoop = bLoop;
	m_kPos = pos;
	m_kDir = dir;
	m_b2DSound = false;

	m_pkResource = NULL;
	m_bLoopingNoLongerHearable = false;

	m_iID = 0;
}

ZFSoundInfo::~ZFSoundInfo()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//																									  //
// ZFAudioSystem																				  //
//																									  //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ZFAudioSystem::ZFAudioSystem(int uiMaxCachSize) : ZFSubSystem("ZFAudioSystem") 
{
	m_kPos = Vector3(0,0,0);

	m_bIsValid = false;
	m_uiCurrentCachSize = 0;
	m_uiMaxCachSize = uiMaxCachSize;
	m_fReferenceDistance = 1.0f;
	
	
	m_iMusicID 			= -1;
	m_fMusicFade 		= 0;
	m_strCurrentMusic	= "";
	m_strNextMusic		= "";
	m_bMusicChange 	= false;
	m_fMusicGain		= 1;
	
	//m_bEnableSound = true;
	//m_bEnableMusic = true;

	//RegisterVariable("a_enablesound",&m_bEnableSound,CSYS_BOOL);
	//RegisterVariable("a_enablemusic",&m_bEnableMusic,CSYS_BOOL);
// 	RegisterVariable("a_soundrefdist",&m_fReferenceDistance,CSYS_FLOAT);

	m_fMainVolume = 1.0f;
	RegisterVariable("a_mainvolume",&m_fMainVolume,CSYS_FLOAT);
	RegisterVariable("a_musicvolume",&m_fMusicGain,CSYS_FLOAT);

	m_pEntityMan = static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));
	m_pkZeroFps  = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
}

ZFAudioSystem::~ZFAudioSystem()
{

}

///////////////////////////////////////////////////////////////////////////////
// Leta reda på det närmsta ljudet och ta bort det.
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::StopAudio(string strName, Vector3 pos, bool bRelease)
{
	ZFSoundInfo *pkSound = GetClosest(strName.c_str(), pos);

	if(pkSound != NULL)
	{
		if(DeleteSound(pkSound, true))
		{
			return true;
		}
	}
	else
	{
		vector<OggStream*>::iterator it = m_vkOggStreams.begin();
		for( ; it!=m_vkOggStreams.end(); it++)
		{
			OggStream* ogg = (*it);
			if(ogg->m_strFileName == strName)
			{
				ogg->Stop();
				if( ogg->m_pkThread != NULL) 
				{
					//SDL_KillThread(ogg->m_pkThread);
					ogg->m_bKillMe = true;
					SDL_WaitThread(ogg->m_pkThread,NULL);

					ogg->m_pkThread = NULL;
				}

				if(bRelease)
				{
					delete ogg;
					m_vkOggStreams.erase(it); 
				}

				return true;
			}
		}
	}

	printf("Failed to stop sound %s\n", strName.c_str());

	return false;
}

///////////////////////////////////////////////////////////////////////////////
// Leta reda på det närmsta ljudet och ta bort det.
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::StopAudio(int iID, bool bRelease)
{
	ZFSoundInfo *pkSound = NULL;

	map<int,ZFSoundInfo*>::iterator itLoopSound = m_kLoopSoundMap.find(iID);
	if(itLoopSound != m_kLoopSoundMap.end())
	{
		pkSound = itLoopSound->second;
		m_kLoopSoundMap.erase(itLoopSound);
	}
	else
	{
		vector<OggStream*>::iterator it = m_vkOggStreams.begin();
		for( ; it!=m_vkOggStreams.end(); it++)
		{
			OggStream* ogg = (*it);
			if(ogg->m_iID == iID)
			{
				ogg->Stop();
				if( ogg->m_pkThread != NULL) 
				{
					ogg->m_bKillMe = true;
					SDL_WaitThread(ogg->m_pkThread,NULL);
					//SDL_KillThread(ogg->m_pkThread);
					ogg->m_pkThread = NULL;
				}

				if(bRelease)
				{
					delete ogg;
					m_vkOggStreams.erase(it); 
				}

				return true;
			}
		}
	}

	if(pkSound == NULL)
	{
		list<ZFSoundInfo*>::iterator itSound = m_kSoundList.begin();
		for( ; itSound != m_kSoundList.end(); itSound++)  
		{
			if((*itSound)->m_iID == iID)
			{
				pkSound = (*itSound);
				break;
			}
		}
	}

	if(pkSound != NULL)
	{
		if(DeleteSound(pkSound, true))
		{
			return true;
		}
	}

	return false;
}

int ZFAudioSystem::PlayAudio(string strName, Vector3 kPos, Vector3 kDir, int iFlags, float fGain)
{
	if(strName.find(".wav") != string::npos)
	{
		ZFSoundInfo *pkSound = new ZFSoundInfo(strName.c_str(), kPos, kDir, iFlags & ZFAUDIO_LOOP);
		
		if(!InitSound(pkSound))
		{
			delete pkSound; // delete sound again.
			return -1;
		}

		g_iIDCounter++;
		pkSound->m_iID = g_iIDCounter;
		pkSound->m_fGain = fGain;
		
		// Lägg till ljudet till vektorn med ljud.
		m_kSoundList.push_back( pkSound );

		if(iFlags & ZFAUDIO_2D)
		{
			pkSound->m_b2DSound = true;
			pkSound->m_kPos = Vector3(0,0,0);
			pkSound->m_kDir = Vector3(0,0,0);
		}

		if(iFlags & ZFAUDIO_LOOP)
			m_kLoopSoundMap.insert(map<int,ZFSoundInfo*>::value_type(pkSound->m_iID, pkSound));
	}
	else
	{
		OggStream* pkOgg;

		bool bExist = false;

		for(int i=0; i<m_vkOggStreams.size(); i++)
		{
			if(m_vkOggStreams[i]->m_strFileName == strName)
			{
				pkOgg = m_vkOggStreams[i];
				bExist = true;
			}
		}

		if(bExist == false)
			pkOgg = new OggStream(!(iFlags & ZFAUDIO_3DOGG),24,4096);
			//pkOgg = new OggStream(!(iFlags & ZFAUDIO_3DOGG),24,8192);

		if(!pkOgg->Open(strName))
		{
			delete pkOgg;
			return -1;
		}

		g_iIDCounter++;
		pkOgg->m_iID = g_iIDCounter;
		pkOgg->SetLooping(iFlags & ZFAUDIO_LOOP);
		if(iFlags & ZFAUDIO_3DOGG) 
			pkOgg->m_kPos = kPos;
		//else
		pkOgg->SetVolume(fGain); // minska volymen eftersom den är för stark i relation till 3dljuden

		pkOgg->Play();

		if(bExist == false)
			m_vkOggStreams.push_back(pkOgg);

		m_vkOggStreams.back()->m_pkThread = 
			SDL_CreateThread(OggStream::ThreadMain, m_vkOggStreams.back());

		
	}

	return g_iIDCounter;
}

/*bool ZFAudioSystem::StopMusic(string strName, bool bRelease)
{
	vector<OggStream*>::iterator it = m_vkOggStreams.begin();
	for( ; it!=m_vkOggStreams.end(); it++)
	{
		OggStream* ogg = (*it);
		if(ogg->m_strFileName == strName)
		{
			ogg->Stop();
			if( ogg->m_pkThread != NULL) 
			{
				SDL_KillThread(ogg->m_pkThread);
				ogg->m_pkThread = NULL;
			}

			if(bRelease)
			{
				delete ogg;
				m_vkOggStreams.erase(it); 
			}

			return true;
		}
	}

	return false;
}*/


///////////////////////////////////////////////////////////////////////////////
// Ladda in ett nytt ljud via resurssystemet.
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::LoadSound(string strFileName)
{
	ZFResourceHandle* pkResHandle = GetResHandle(strFileName);

	if(pkResHandle->IsValid() == false)
	{
		if(pkResHandle->SetRes( strFileName ))
		{
			int size = pkResHandle->GetResourcePtr()->CalculateSize();
			
			m_uiCurrentCachSize += size; // öka på cachen.

			// Ladda ur den minst använda resursen om cachen är fylld.
			if(m_uiCurrentCachSize > m_uiMaxCachSize)
			{
				string strSoundToRemove;

				if( GetSoundWithLowestPriority(strSoundToRemove) )
					if(!UnLoadSound(strSoundToRemove))
						return false;
			}

			/*printf("Loaded sound: %s (index:%i, %i bytes), cach size = %i\n", 
				strFileName.c_str(), 
				((ZFSoundRes*) pkResHandle->GetResourcePtr())->GetBufferIndexName(),
				size, m_uiCurrentCachSize );*/

			return true;
		}
		else
		{
			cout<<"failed to load sound:"<<strFileName<<endl;
		}
	}
	else
	{
		// Already exist.
		return true;
	}

	// Vi har misslyckats ladda in ljudet så vi tar bor resurshantaget... 031205
	UnLoadSound(strFileName);

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// Ladda ur ett ljud (via resurssystemet).
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::UnLoadSound(string strFileName)
{
	map<string,tResPriorHandle>::iterator itRes;
	itRes = m_mkResHandles.find(strFileName);
	if(itRes != m_mkResHandles.end())
	{
		ZFResourceHandle* pkResHandle = itRes->second.first;

		if(pkResHandle->IsValid() == true)
		{
			int size = pkResHandle->GetResourcePtr()->CalculateSize();
				
			m_uiCurrentCachSize -= size; // minska ner cachen.
			if(m_uiCurrentCachSize < 0)
				m_uiCurrentCachSize = 0;

			printf("UnLoading sound: %s (index:%i, %i bytes), cach size = %i\n", 
				strFileName.c_str(), 
				((ZFSoundRes*) pkResHandle->GetResourcePtr())->GetBufferIndexName(),
				size, m_uiCurrentCachSize );

			// Ta bort alla ljus i systemet som använder resursen
			vector<ZFSoundInfo*> kSoundsUsingRes;
			GetSoundsUsingResource((ZFSoundRes*)pkResHandle->GetResourcePtr(), 
				kSoundsUsingRes);

			for(unsigned int i=0; i<kSoundsUsingRes.size(); i++)
				DeleteSound(kSoundsUsingRes[i], true);

			m_mkResHandles.erase(itRes); // remove from map

			delete pkResHandle; // laddu ur resursen.

			return true;
		}
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// Ladda ur alla ljud
///////////////////////////////////////////////////////////////////////////////
void ZFAudioSystem::UnloadAll()
{
	// Remove resurce files.
	map<string,tResPriorHandle>::iterator itRes = m_mkResHandles.begin();
	for( ; itRes != m_mkResHandles.end(); itRes++)
		if(itRes->second.first)
		{
			vector<ZFSoundInfo*> kSoundsUsingRes;
			GetSoundsUsingResource((ZFSoundRes*)itRes->second.first->GetResourcePtr(), 
				kSoundsUsingRes);

			for(unsigned int i=0; i<kSoundsUsingRes.size(); i++)
				DeleteSound(kSoundsUsingRes[i], true);

			delete itRes->second.first; // unload resource.
		}

	m_uiCurrentCachSize = 0;
	m_mkResHandles.clear();
}


///////////////////////////////////////////////////////////////////////////////
// Kolla hur många ljud som för tillfället hanteras av systemet.
///////////////////////////////////////////////////////////////////////////////
unsigned int ZFAudioSystem::GetNumSounds()
{
	return m_kSoundList.size();
}


///////////////////////////////////////////////////////////////////////////////
// Kolla hur många ljud som för tillfället spelas upp av systemet.
///////////////////////////////////////////////////////////////////////////////
unsigned int ZFAudioSystem::GetNumActiveChannels()
{
	unsigned int uiAntal = 0;

	list<ZFSoundInfo*>::iterator itSound = m_kSoundList.begin();
	for( ; itSound != m_kSoundList.end(); itSound++)  
		if((*itSound)->m_uiSourceBufferName != NO_SOURCE)
			uiAntal++;

	return uiAntal;
}


///////////////////////////////////////////////////////////////////////////////
// Starta upp systemet.
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::StartUp()
{
	Register_Cmd("audioplay",FID_AUDIOPLAY);
	Register_Cmd("audiostop",FID_AUDIOSTOP);

	alutInit (0, NULL); 
	alGetError();
	alListenerf(AL_GAIN, 1.0f);
	alDopplerFactor(0.0f);
	alDopplerVelocity(343); 

	SetListnerPosition(Vector3(0,0,0),Vector3(0,1,0),Vector3(0,1,0));

	EntityManager* pkObjectMan = reinterpret_cast<EntityManager*>(
		g_ZFObjSys.GetObjectPtr("EntityManager"));

	ZFScriptSystem* pkScriptSys = reinterpret_cast<ZFScriptSystem*>(
		GetSystem().GetObjectPtr("ZFScriptSystem"));

	// Setup script interface.
	AudioLua::Init(this,pkObjectMan,pkScriptSys);

	m_bIsValid = true;

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Stäng ner systemet.
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::ShutDown()
{
	for(int i=0; i<m_kAmbientAreas.size(); i++)
		for(int j=0; j<m_kAmbientAreas[i]->m_kPolygon.size(); j++)
			delete m_kAmbientAreas[i]->m_kPolygon[j];

	for(int i=0; i<m_vkOggStreams.size(); i++)
	{
		if( m_vkOggStreams[i]->m_pkThread != NULL)
		{
			//SDL_KillThread(m_vkOggStreams[i]->m_pkThread);
			m_vkOggStreams[i]->m_bKillMe = true;
			SDL_WaitThread(m_vkOggStreams[i]->m_pkThread,NULL);
		}

		delete m_vkOggStreams[i];
	}

	// Destroy all resources and sounds.
	UnloadAll();

	// Destroy OpenAL (very, very, fucking importent)
	alutExit();

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Undersök om systemet är initierat.
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::IsValid()
{
	return m_bIsValid;
}


///////////////////////////////////////////////////////////////////////////////
// Kör kommandon från konsolen.
///////////////////////////////////////////////////////////////////////////////
void ZFAudioSystem::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) 
	{
		case FID_AUDIOPLAY:
			if(kCommand->m_kSplitCommand.size() > 1)
				PlayAudio(kCommand->m_kSplitCommand[1], Vector3(), Vector3(), ZFAUDIO_LOOP | ZFAUDIO_2D);
			break;
		
		case FID_AUDIOSTOP:
			if(kCommand->m_kSplitCommand.size() > 1)
				StopAudio(kCommand->m_kSplitCommand[1], Vector3(0,0,0));
			break;
		
		//case FID_MUSICVOLUME:
		//	if(kCommand->m_kSplitCommand.size() > 1)
		//	{
		//		m_fMusicVolume = (float)strtod( kCommand->m_kSplitCommand[2].c_str(), NULL );
		//		m_pkMusic->SetVolume(m_fMusicVolume);
		//	}
		//	break;

		//case FID_SOUNDVOLUME:
		//	if(kCommand->m_kSplitCommand.size() > 1)
		//	{
		//		m_fSoundVolume = (float)strtod( kCommand->m_kSplitCommand[1].c_str(),NULL );
		//		SetSoundVolume(m_fMusicVolume);
		//	}
		//	break;
	}
}

//bool ZFAudioSystem::SetSoundVolume(float fVolume)
//{
//	if((fVolume<=1.0) && (fVolume>0.0))
//	{
//		m_fSoundVolume = fVolume;
//		return true;
//	}
//	return false;
//};


///////////////////////////////////////////////////////////////////////////////
// Uppdatera ljudsystemet. Anropas från mainloopen.
///////////////////////////////////////////////////////////////////////////////
void ZFAudioSystem::Update()
{
	//if(m_bEnableSound == false)
	//	return;
	
	//dvoid waz here and hacked a bit =D
	//sätter ljudvolym till noll om applicationen är minimerad
	if(m_pkZeroFps->GetMinimized())
		alListenerf(AL_GAIN, 0.0);
	else	
		alListenerf(AL_GAIN, m_fMainVolume);
	
	UpdateAmbientSound();

	// Temporär vektor som fylls med det ljud som inte längre kan höras
	// eller som har stannat.
	vector<ZFSoundInfo*> kStopped;

	// Temporär vektor med loop ljud som skall startas om.
	vector<ZFSoundInfo*> kRestart;

	list<ZFSoundInfo*>::iterator itSound = m_kSoundList.begin();
	for( ; itSound != m_kSoundList.end(); itSound++)  
	{
		ZFSoundInfo* pkSound = (*itSound);

		bool bHearable = Hearable( pkSound );

		if( pkSound->m_bLoopingNoLongerHearable && bHearable)
		{
			kRestart.push_back(pkSound); 
			continue;
		}

		ALint iSourceState;
		alGetSourcei(pkSound->m_uiSourceBufferName, 
			AL_SOURCE_STATE, &iSourceState);

		switch(iSourceState)
		{
		case AL_INITIAL: // Initierat.
			if(bHearable)
				Play(pkSound);
			break;

		case AL_PLAYING: // Spelar.
			if(bHearable)
			{
				if(pkSound->m_b2DSound == false)
				{
					alSourcefv(pkSound->m_uiSourceBufferName, AL_POSITION,&pkSound->m_kPos[0]);	
					alSourcefv(pkSound->m_uiSourceBufferName, AL_VELOCITY,&pkSound->m_kDir[0]);
				}
				else
				{
					alSource3f(pkSound->m_uiSourceBufferName, AL_POSITION,        0.0, 0.0, 0.0);
					alSource3f(pkSound->m_uiSourceBufferName, AL_VELOCITY,        0.0, 0.0, 0.0);
					alSource3f(pkSound->m_uiSourceBufferName, AL_DIRECTION,       0.0, 0.0, 0.0);
					alSourcef (pkSound->m_uiSourceBufferName, AL_ROLLOFF_FACTOR,  0.0          );
					alSourcei (pkSound->m_uiSourceBufferName, AL_SOURCE_RELATIVE, AL_TRUE      );
				}

				float fGain = m_fMainVolume * pkSound->m_fGain;
				alSourcef(pkSound->m_uiSourceBufferName, AL_GAIN, fGain/*pkSound->m_fGain*/);
			}
			else
			{
				kStopped.push_back(pkSound); 

				if(pkSound->m_bLoop)
					pkSound->m_bLoopingNoLongerHearable = true;
			}
			break;

		case AL_STOPPED: // Stannat.
			if(pkSound->m_bLoop == false)
				kStopped.push_back(pkSound);
			break;
		}
	}

	// Stoppa färdiga ljud.
	for(int i=0; i<kStopped.size(); i++)
		Stop(kStopped[i]);

	// Start om loopade ljud.
	for(int i=0; i<kRestart.size(); i++)
	{
		if(Restart(kRestart[i]) == false)
		{
			DeleteSound(kRestart[i], true);
		}
	}
	
	
	UpdateMusic();
}


///////////////////////////////////////////////////////////////////////////////
// Sätt en ny position och orientering på lysnaren.
///////////////////////////////////////////////////////////////////////////////
void ZFAudioSystem::SetListnerPosition(Vector3 kPos,Matrix4 kOri)
{	
	kOri.Transponse();
	Vector3 up = -kOri.GetAxis(1);
	Vector3 dir = Vector3(0,0,1);
	dir = kOri.VectorRotate(dir);

	SetListnerPosition(kPos,dir,up);
}

void ZFAudioSystem::SetListnerPosition(Vector3 kPos,Matrix3 kOri)
{	
	kOri.Transponse();
	Vector3 up = -kOri.GetAxis(1);
	Vector3 dir = Vector3(0,0,1);
	dir = kOri.VectorTransform(dir);

	SetListnerPosition(kPos,dir,up);
}

void ZFAudioSystem::SetListnerPosition(Vector3 kPos,Vector3 kHead,Vector3 kUp) 
{
	m_kPos=kPos;
	m_kHead=kHead;
	m_kUp=kUp;

	alListenerfv(AL_POSITION, &kPos[0]);
	
	float orientation[]={kHead.x,kHead.y,kHead.z, kUp.x,kUp.y,kUp.z};
	
	float listenerVel[] = {0,0,0};
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, orientation);
}


///////////////////////////////////////////////////////////////////////////////
// Skriv ut eff felmeddelande i konsolfönstret.
///////////////////////////////////////////////////////////////////////////////
void ZFAudioSystem::PrintError(ALenum error, char *szDesc)
{
	printf("%s Error: \n", szDesc);

/*	switch(error)
	{
	case AL_INVALID_NAME:
		printf(" AL_INVALID_NAME\n");
		break;
//	case AL_INVALID_ENUM:
//		printf(" AL_INVALID_ENUM\n");
//		break;
	case AL_INVALID_VALUE:
		printf(" AL_INVALID_VALUE\n");
		break;
//	case AL_INVALID_OPERATION:
//		printf(" AL_INVALID_OPERATION\n");
//		break;
	case AL_OUT_OF_MEMORY:
		printf(" AL_OUT_OF_MEMORY\n");
		break;
	}*/
}



///////////////////////////////////////////////////////////////////////////////
// Returnerar ett resurshantag om ett sådant finns eller skapar
// ett nytt och sparar ner det i en map om inget finns.
///////////////////////////////////////////////////////////////////////////////
ZFResourceHandle* ZFAudioSystem::GetResHandle(string strFileName)
{	
	// Check if resource handle exist.
	map<string,tResPriorHandle>::iterator itRes;
	itRes = m_mkResHandles.find(strFileName);
	if(itRes != m_mkResHandles.end())
		return itRes->second.first; // return if exist.

	// Add resource if not.
	ZFResourceHandle* pkNewRes = new ZFResourceHandle;
	m_mkResHandles.insert(map<string,tResPriorHandle>::value_type(
		strFileName, tResPriorHandle(pkNewRes,1))); // sätt prioritet till 1

	return pkNewRes;
}


///////////////////////////////////////////////////////////////////////////////
// Returnerar ett resurshantags prioritets värde.
///////////////////////////////////////////////////////////////////////////////
int ZFAudioSystem::GetResHandlePriority(string strFileName)
{	
	// Check if resource handle exist.
	map<string,tResPriorHandle>::iterator itRes;
	itRes = m_mkResHandles.find(strFileName);
	if(itRes != m_mkResHandles.end())
		return itRes->second.second; // return if exist.

	return 0;
}



///////////////////////////////////////////////////////////////////////////////
// Modifiera ett resurshantags prioritet värde.
///////////////////////////////////////////////////////////////////////////////
int ZFAudioSystem::ModifyResHandlePriority(string strFileName, int mod)
{	
	// Check if resource handle exist.
	map<string,tResPriorHandle>::iterator itRes;
	itRes = m_mkResHandles.find(strFileName);
	if(itRes != m_mkResHandles.end())
	{
		itRes->second.second += mod;
		return itRes->second.second; // return if exist.
	}

	return 0;
}



///////////////////////////////////////////////////////////////////////////////
// Returnerar ett resurshantags prioritets värde.
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::GetSoundWithLowestPriority(string& strRes)
{	
	// Check if resource handle exist.
	map<string,tResPriorHandle>::iterator it = m_mkResHandles.begin();

	const int DONT_EXIST = 1000000;
	int iLowestPriority = DONT_EXIST;
	string strLowest;

	// Loopa igen alla resurser och hitta den resurs som har lägst prioritet
	for( ; it != m_mkResHandles.end(); it++)
	{
		if( it->second.second < iLowestPriority )
		{
			iLowestPriority = it->second.second;
			strLowest = it->first;
		}
	}
	
	if(iLowestPriority == DONT_EXIST)
		return false;

	strRes = strLowest;

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Undersök om ett ljud är går att höra.
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::Hearable(ZFSoundInfo* pkSound)
{
	//dvoid la till detta...blev massa felutskrifter annars o verka itne fungera korrekt
	if( pkSound->m_b2DSound)
	{
		return true;
	}

	if( pkSound->m_kPos.DistanceTo(m_kPos) < HEARABLE_DISTANCE )
		return true;

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// Leta reda på närmsta ljud med det namnet och den positionen
///////////////////////////////////////////////////////////////////////////////
ZFSoundInfo* ZFAudioSystem::GetClosest(const char* szName, Vector3 kPos)
{
	ZFSoundInfo* pkClosest = NULL;
	float fClosestDist = 100000.0f;

	list<ZFSoundInfo*>::iterator itSound = m_kSoundList.begin();
	for( ; itSound != m_kSoundList.end(); itSound++)  
	{
		ZFSoundInfo* pkSound = (*itSound);

		if(strcmp(pkSound->m_acFile, szName) == 0)
		{
			float fDistance = (kPos - pkSound->m_kPos).LengthSqr();
			if(fDistance < fClosestDist)
			{
				pkClosest = pkSound;
				fClosestDist = fDistance;
			}
		}
	}

	return pkClosest;
}


void ZFAudioSystem::GetSoundsUsingResource(ZFSoundRes* pkResource, 
														 vector<ZFSoundInfo*>& vkSounds)
{
	list<ZFSoundInfo*>::iterator it = m_kSoundList.begin();
	for( ; it != m_kSoundList.end(); it++)
	{
		ZFSoundInfo* pkSound = (*it);

		if(pkSound->m_pkResource == pkResource)
			vkSounds.push_back(pkSound);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Radera ljudkälla (source) sammt eventuellt ljudbuffert.
// Tar bort ljudet ur systemet om andra argumetet är satt till true.
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::DeleteSound(ZFSoundInfo *pkSound, bool bRemoveFromSystem)
{
	ALenum error = AL_NO_ERROR;


	// Stoppa ljudet.
	alGetError(); // clear
	alSourceStop(pkSound->m_uiSourceBufferName);
// 	if( (error = alGetError()) != AL_NO_ERROR)
// 	{
// 		ZFAudioSystem::PrintError(error, "Failed to stop sound before delete!"); 
// 		printf("name = %i\n", pkSound->m_uiSourceBufferName);
// 	}

	// Deallokera source pekaren.
	alGetError(); // clear
	alDeleteSources(1, &pkSound->m_uiSourceBufferName);
// 	if( (error = alGetError()) != AL_NO_ERROR)
// 		ZFAudioSystem::PrintError(error, "Failed to delete source buffer!"); 

	pkSound->m_uiSourceBufferName = NO_SOURCE;

	// Minska prioritets värdet med 1.
	ModifyResHandlePriority( string(pkSound->m_acFile), -1);

	// Ta bort ljudet ur systemet.
	if(bRemoveFromSystem)
	{
		m_kSoundList.remove( pkSound ) ;
		delete pkSound; // remove from system
	}
	else
		pkSound->m_pkResource = NULL;

	if( error == AL_NO_ERROR )
	{
		//printf("stopping sound\n");
		return true;
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// Undersöker om det inte finns något annat ljud i systemet som använder
// samma ljudbuffert (dvs. resurs).
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::ResourceIsUnused(ZFSoundInfo* pkSound)
{
	if(pkSound == NULL)
		return false;

	list<ZFSoundInfo*>::iterator it = m_kSoundList.begin();
	for( ; it != m_kSoundList.end(); it++)
	{
		if((*it) != pkSound)
		{
			if((*it)->m_pkResource == pkSound->m_pkResource)
				return false;
		}
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Försök spela upp ett ljud.
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::Play(ZFSoundInfo *pkSound)
{
	ALenum error;

	if(pkSound->m_pkResource == NULL)
	{
		printf("ZFAudioSystem::Play, Trying to play a sound without a resource!\n");
		return false;
	}

	unsigned int buffer = pkSound->m_pkResource->GetBufferIndexName();

	if(buffer == 0)
	{
		printf("ZFAudioSystem::Play, Bad buffer index: %s", 
			"ZFSoundRes::GetBufferIndexName() == 0!\n");
		return false;
	}

	// Set buffer.
	alGetError();
	alSourcei(pkSound->m_uiSourceBufferName, AL_BUFFER, buffer );	
	if( (error = alGetError()) != AL_NO_ERROR)
	{
		PrintError(error, "ZFAudioSystem::Play, Failed to set buffer!");
		return false;
	}
	
	// Set reference distance.
	alGetError();
	alSourcef(pkSound->m_uiSourceBufferName, AL_REFERENCE_DISTANCE, m_fReferenceDistance);

	if( (error = alGetError()) != AL_NO_ERROR)
	{
		PrintError(error, "ZFAudioSystem::Play, Failed to set reference distance!");
		return false;
	}
	
	// Set gain.
	alGetError();
	float fGain = m_fMainVolume * pkSound->m_fGain;
	alSourcef(pkSound->m_uiSourceBufferName, AL_GAIN, fGain/*pkSound->m_fGain*/);
	if( (error = alGetError()) != AL_NO_ERROR)
	{
		PrintError(error, "ZFAudioSystem::Play, Failed to set gain!");
		return false;
	}

	if(!pkSound->m_b2DSound)
	{
		// Set position.
		alGetError();
		alSourcefv(pkSound->m_uiSourceBufferName, AL_POSITION, &pkSound->m_kPos[0]);	
		if( (error = alGetError()) != AL_NO_ERROR)
		{
			PrintError(error, "ZFAudioSystem::Play, Failed to set position!");
			return false;
		}
		
		// Set velocity.
		alGetError();
		alSourcefv(pkSound->m_uiSourceBufferName, AL_VELOCITY, &pkSound->m_kDir[0]);
		if( (error = alGetError()) != AL_NO_ERROR)
		{
			PrintError(error, "ZFAudioSystem::Play, Failed to set velocity!");
			return false;
		}
	}

	// Set loop mode.
	alGetError();
	alSourcei(pkSound->m_uiSourceBufferName, AL_LOOPING, pkSound->m_bLoop ? 1 : 0);
	if( (error = alGetError()) != AL_NO_ERROR)
	{
		PrintError(error, "ZFAudioSystem::Play, Failed to set loop mode!");
		return false;
	}

	// Play.
	alGetError(); // Clear Error Code
	alSourcePlay(pkSound->m_uiSourceBufferName);
	if( (error = alGetError()) != AL_NO_ERROR)
	{
		PrintError(error, "ZFAudioSystem::Play, Failed to call alSourcePlay!");
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Försök stoppa ett ljud. Är det ett vanligt ljud så tas det ur systemet
// (men resursen behålls innladdad ifall fler ljud använder den). Är det ett 
// loopat ljud så behålls ljudet i systemet men ljudkällan (source) förstörs
// så att andra ljud kan användas på den lediga ljudkanalen.
///////////////////////////////////////////////////////////////////////////////
void ZFAudioSystem::Stop(ZFSoundInfo *pkSound)
{
	if(pkSound->m_uiSourceBufferName == NO_SOURCE)
		return; // already stoped.

	if(pkSound->m_bLoop)
		DeleteSound(pkSound, false);
	else
		DeleteSound(pkSound, true);
}


///////////////////////////////////////////////////////////////////////////////
// Starta om ett ljud som redan finns i systemet.
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::Restart(ZFSoundInfo *pkSound)
{
	//printf("restarting sound\n");
	
	if(!InitSound(pkSound))
		return false;
	
	if(pkSound->m_bLoop)
		pkSound->m_bLoopingNoLongerHearable = false;

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Ladda in ett nytt ljud om det behövs och generera en ny source buffer.
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::InitSound(ZFSoundInfo *pkSound)
{
	string strName = pkSound->m_acFile;

	ZFResourceHandle* pkResHandle = GetResHandle(strName);

	// Ladda in om det behövs.
	if(pkResHandle->IsValid() == false)
	{
		if(!LoadSound(strName))
			return false;
	}
	
	// Öka prioritets värdet med 1.
	ModifyResHandlePriority(strName, 1);

	// Sätt resurs pekaren.
	pkSound->m_pkResource = 
		reinterpret_cast<ZFSoundRes*>(pkResHandle->GetResourcePtr());

	pkSound->m_uiSourceBufferName = GetNewSource();

	// Generera en ny source buffer.
	//ALenum error;
	//alGetError();
	//alGenSources(1, &pkSound->m_uiSourceBufferName);
	//if( (error = alGetError()) != AL_NO_ERROR)
	//{
	//	ZFSoundInfo* pkFarest = NULL;
	//	float fFarest = 0.0f;

	//	// Loopa igenom alla aktiva ljud och ta bort det som är längst bort 
	//	list<ZFSoundInfo*>::iterator it = m_kSoundList.begin();
	//	for( ; it != m_kSoundList.end(); it++)
	//	{
	//		ZFSoundInfo* pkSound = (*it);
	//		
	//		float fDistance = (m_kPos - pkSound->m_kPos).LengthSqr();
	//		if(fDistance > fFarest)
	//		{
	//			pkFarest = pkSound;
	//			fFarest = fDistance;
	//		}
	//	}

	//	if(pkFarest != NULL)
	//	{
	//		DeleteSound(pkFarest, true);
	//		return InitSound(pkSound);
	//	}

	//	PrintError(error, "Failed to generate sound source!\n");
	//		
	//	return false;
	//}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Försök generera en ny "ljudkanal", ta bort det ljud som är längst bort från
// lyssnaren om alla kanaler redan är använda.
///////////////////////////////////////////////////////////////////////////////

ALuint ZFAudioSystem::GetNewSource()
{
	ALuint uiNewSource = -1;

	ALenum error;
	alGetError();
	alGenSources(1, &uiNewSource);
	if( (error = alGetError()) == AL_NO_ERROR)
		return uiNewSource;
	else
	{
		ZFSoundInfo* pkFarest = NULL;
		float fFarest = 0.0f;

		// Loopa igenom alla aktiva ljud och ta bort det som är längst bort 
		list<ZFSoundInfo*>::iterator it = m_kSoundList.begin();
		for( ; it != m_kSoundList.end(); it++)
		{
			ZFSoundInfo* pkSound = (*it);
			
			float fDistance = (m_kPos - pkSound->m_kPos).LengthSqr();
			if(fDistance > fFarest)
			{
				pkFarest = pkSound;
				fFarest = fDistance;
			}
		}

		if(pkFarest != NULL)
		{
			DeleteSound(pkFarest, true);
			return GetNewSource();
		}

		PrintError(error, "Failed to generate sound source!\n");
			
		return -1;
	}
}


///////////////////////////////////////////////////////////////////////////////
// Leta rätt på det närmsta ljudet med det namnet och flytta på det till ny postion
///////////////////////////////////////////////////////////////////////////////
/*
bool ZFAudioSystem::MoveSound(const char* szName, Vector3 kOldPos, Vector3 kNewPos,
										Vector3 kNewDir)
{
	ZFSoundInfo* pkClosest = NULL;
	float fClosestDist = 100000.0f;

	list<ZFSoundInfo*>::iterator itFind = m_kSoundList.end();

	list<ZFSoundInfo*>::iterator itSound = m_kSoundList.begin();
	for( ; itSound != m_kSoundList.end(); itSound++)  
	{
		ZFSoundInfo* pkSound = (*itSound);

		if(strcmp(pkSound->m_acFile, szName) == 0)
		{
			float fDistance = (kOldPos - pkSound->m_kPos).LengthSqr();
			if(fDistance < fClosestDist)
			{
				pkClosest = pkSound;
				fClosestDist = fDistance;
				itFind = itSound;
			}
		}
	}

	if(itFind != m_kSoundList.end())
	{
		(*itFind)->m_kPos = kNewPos;
		(*itFind)->m_kDir = kNewDir;
		return true;
	}

	return false;
}
*/



///////////////////////////////////////////////////////////////////////////////
// Leta rätt på ljudet med det ID:t och flytta på det till ny postion
///////////////////////////////////////////////////////////////////////////////

bool ZFAudioSystem::MoveAudio(int iID, Vector3 kNewPos, Vector3 kNewDir, float fVolume)
{
	map<int,ZFSoundInfo*>::iterator itLoopSound = m_kLoopSoundMap.find(iID);
	if(itLoopSound != m_kLoopSoundMap.end())
	{
		itLoopSound->second->m_kPos = kNewPos;
		itLoopSound->second->m_kDir = kNewDir;
		if(fVolume > 0)
			itLoopSound->second->m_fGain = fVolume;
		return true;
	}
	else
	{
		vector<OggStream*>::iterator it = m_vkOggStreams.begin();
		for( ; it!=m_vkOggStreams.end(); it++)
		{
			OggStream* ogg = (*it);
			if(ogg->m_iID == iID)
			{
				ogg->m_kPos = kNewPos;
				if(fVolume > 0)
					ogg->SetVolume(fVolume);
				return true;
			}
		}
	}

	list<ZFSoundInfo*>::iterator itFind = m_kSoundList.end();

	list<ZFSoundInfo*>::iterator itSound = m_kSoundList.begin();
	for( ; itSound != m_kSoundList.end(); itSound++)  
	{
		if((*itSound)->m_iID == iID)
		{
			itFind = itSound;
			break;
		}
	}

	if(itFind != m_kSoundList.end())
	{
		(*itFind)->m_kPos = kNewPos;
		(*itFind)->m_kDir = kNewDir;

		if(fVolume > 0)
		{
			(*itFind)->m_fGain = fVolume;
		}

		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
// Leta rätt på ljudet med det ID:t och ändra gaoim på det 
///////////////////////////////////////////////////////////////////////////////

bool ZFAudioSystem::SetGain(int iID, float fGain)
{
/*
	list<ZFSoundInfo*>::iterator itFind = m_kSoundList.end();

	list<ZFSoundInfo*>::iterator itSound = m_kSoundList.begin();
	for( ; itSound != m_kSoundList.end(); itSound++)  
	{
		if((*itSound)->m_iID == iID)
		{
			itFind = itSound;
			break;
		}
	}

	if(itFind != m_kSoundList.end())
	{
		(*itFind)->m_fGain = fGain;
		return true;
	}

	return false;
	*/

	ZFSoundInfo *pkSound = NULL;

	map<int,ZFSoundInfo*>::iterator itLoopSound = m_kLoopSoundMap.find(iID);
	if(itLoopSound != m_kLoopSoundMap.end())
	{
		pkSound = itLoopSound->second;
		pkSound->m_fGain = fGain;
	}
	else
	{
		vector<OggStream*>::iterator it = m_vkOggStreams.begin();
		for( ; it!=m_vkOggStreams.end(); it++)
		{
			OggStream* ogg = (*it);
			if(ogg->m_iID == iID)
			{
				ogg->SetVolume(fGain);
				return true;
			}
		}
	}

	if(pkSound == NULL)
	{
		//printf("Failed to set Gain %d for sound: %i \n",fGain,iID);
// 		cout<<"Failed to set Gain "<<fGain<<" for sound:"<<iID<<endl;
	}

	return false;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ambient Sound Area
////////////////////////////////////////////////////////////////////////////////////////////////////////

const float FADE_TIME_AMBIENT_AREA = 2.5f; // sekunder.

void ZFAudioSystem::UpdateAmbientSound()
{
	for(int i=0; i<m_kAmbientAreas.size(); i++)
	{
		if(!m_kAmbientAreas[i]->m_strSound.empty())
		{
			Vector2 pos(m_kPos.x, m_kPos.z);

				if( (m_kPos.y >= m_kAmbientAreas[i]->m_fFloor && 
					  m_kPos.y <= m_kAmbientAreas[i]->m_fRoof) &&
					  PntInPolygon(&pos, m_kAmbientAreas[i]->m_kPolygon) )
				{
					if(m_kAmbientAreas[i]->m_bChangeSound)
					{
						bool bStartNewSound = true;
						for(int j=0; j<m_kAmbientAreas.size(); j++)
						{
							if(i != j && (m_kAmbientAreas[j]->m_strSound == m_kAmbientAreas[i]->m_strSound) )
							{
								if(m_kAmbientAreas[j]->m_iSoundID > 0)
								{
									bStartNewSound = false;
									m_kAmbientAreas[i]->m_iSoundID = m_kAmbientAreas[j]->m_iSoundID;
									m_kAmbientAreas[i]->m_fGain = m_kAmbientAreas[j]->m_fGain;
									break;
								}
							}
						}

						if(bStartNewSound)
							m_kAmbientAreas[i]->m_iSoundID = PlayAudio(m_kAmbientAreas[i]->m_strSound, 
								m_kPos, m_kHead, true, m_kAmbientAreas[i]->m_fGain); // och starta ett nytt.
						
						m_kAmbientAreas[i]->m_bChangeSound = false;
					}
					else
					{
						if(m_kAmbientAreas[i]->m_fGain < 1)
						{
							FadeGain(m_kAmbientAreas[i], false);
						}
						else
						{
							m_kAmbientAreas[i]->m_fGain = 1.0f;
							m_kAmbientAreas[i]->m_fFadeTimer = -1;
						}
						
						MoveAudio(m_kAmbientAreas[i]->m_iSoundID, m_kPos, m_kHead, m_kAmbientAreas[i]->m_fGain);
					}
				}
				else
				{
					if(m_kAmbientAreas[i]->m_iSoundID > 0)
					{
						if(m_kAmbientAreas[i]->m_fGain > 0)
						{
							FadeGain(m_kAmbientAreas[i], true);
						}
						else
						{
							bool bStopSound = true;

							// Det kan hända att ett annat ljud spelar med detta ID. Stoppa inte ljudet i så fall.
							for(int k=0; k<m_kAmbientAreas.size(); k++)
							{
								if((i != k) && m_kAmbientAreas[k]->m_iSoundID == m_kAmbientAreas[i]->m_iSoundID)
								{
									bStopSound = false;
									break;
								}
							}

							if(bStopSound)
								StopAudio(m_kAmbientAreas[i]->m_iSoundID);

							m_kAmbientAreas[i]->m_iSoundID = -1;
							m_kAmbientAreas[i]->m_bChangeSound = true;
							m_kAmbientAreas[i]->m_fGain = 0.0f;
							m_kAmbientAreas[i]->m_fFadeTimer = -1;
						}
					}
				}
			
		}
	}
}

int ZFAudioSystem::AddAmbientArea(string strSound, vector<Vector2>& kArea, float fFloor, float fRoof)
{
	AmbientArea* pkArea = NULL;

	// Kolla om den redan finns
	for(int i=0; i<m_kAmbientAreas.size(); i++)
	{
		if(m_kAmbientAreas[i]->m_strSound == strSound)
			if(m_kAmbientAreas[i]->m_kPolygon.size() == kArea.size())
			{
				pkArea = m_kAmbientAreas[i];

				for(int j=0; j<kArea.size(); j++)
					if((*m_kAmbientAreas[i]->m_kPolygon[i]) != kArea[j])
					{
						if( m_kAmbientAreas[i]->m_fFloor == fFloor && 
							 m_kAmbientAreas[i]->m_fRoof == fRoof)
						{
							pkArea = NULL;
							break;
						}
					}
			}
	}

	// Skape en ny och lägg till i listan.
	if(pkArea == NULL)
	{
		pkArea = new AmbientArea();
		pkArea->m_iAmbientAreaID = g_iAmbientAreaIDCounter++;
		pkArea->m_strSound = strSound;
		pkArea->m_bChangeSound = true;
		pkArea->m_fGain = 0.0f;
		pkArea->m_fFloor = fFloor;
		pkArea->m_fRoof = fRoof;

		for(int i=0; i<kArea.size(); i++)
			pkArea->m_kPolygon.push_back(new Vector2( kArea[i] ));	
			
		m_kAmbientAreas.push_back(pkArea);

		printf("Adding new ambient area\n");
	}
	
	return pkArea->m_iAmbientAreaID;
}

bool ZFAudioSystem::ChangePntsInAmbientArea(int iID, vector<Vector2>& kArea)
{
	for(int i=0; i<m_kAmbientAreas.size(); i++)
	{
		if(m_kAmbientAreas[i]->m_iAmbientAreaID == iID)
		{
			for(int j=0; j<m_kAmbientAreas[i]->m_kPolygon.size(); j++)
				delete m_kAmbientAreas[i]->m_kPolygon[j];

			m_kAmbientAreas[i]->m_kPolygon.clear();

			for(int j=0; j<kArea.size(); j++)
				m_kAmbientAreas[i]->m_kPolygon.push_back(new Vector2( kArea[j] ));	
			
			return true;
		}
	}

	return false;
}

bool ZFAudioSystem::ChangeAmbientAreaYPos(int iID, float fFloor, float fRoof)
{
	for(int i=0; i<m_kAmbientAreas.size(); i++)
	{
		if(m_kAmbientAreas[i]->m_iAmbientAreaID == iID)
		{
			m_kAmbientAreas[i]->m_fFloor = fFloor;
			m_kAmbientAreas[i]->m_fRoof = fRoof;
			return true;
		}
	}

	return false;
}

bool ZFAudioSystem::ChangeAmbientAreaSound(int iID, string strSound)
{
	for(int i=0; i<m_kAmbientAreas.size(); i++)
	{
		if(m_kAmbientAreas[i]->m_iAmbientAreaID == iID)
		{
			if(m_kAmbientAreas[i]->m_strSound != strSound)
			{
				if(m_kAmbientAreas[i]->m_iSoundID != -1)
					StopAudio(m_kAmbientAreas[i]->m_iSoundID);

				m_kAmbientAreas[i]->m_strSound = strSound;	

				m_kAmbientAreas[i]->m_bChangeSound = true;
				
				printf("chagninge sound to ambient area\n");
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	return false;
}

void ZFAudioSystem::RemoveAmbientArea(int iID)
{
	vector<AmbientArea*>::iterator it = m_kAmbientAreas.begin(); 
	for( ; it != m_kAmbientAreas.end(); it++) 
	{
		if((*it)->m_iAmbientAreaID == iID)
		{
			StopAudio((*it)->m_iSoundID);
			delete (*it);
			m_kAmbientAreas.erase(it);
			break;
		}
	}
}

void ZFAudioSystem::FadeGain(AmbientArea* pkArea, bool bOut)
{
	float fTime = m_pkZeroFps->GetEngineTime();

	if(pkArea->m_fFadeTimer < 0)
		pkArea->m_fFadeTimer = fTime;

	float fTimeSinceLastFrame = fTime - pkArea->m_fFadeTimer;
	float dif = fTimeSinceLastFrame / FADE_TIME_AMBIENT_AREA;

	if(bOut)
		pkArea->m_fGain -= dif;
	else
		pkArea->m_fGain += dif;

	pkArea->m_fFadeTimer = fTime;
}

bool ZFAudioSystem::PntInPolygon(Vector2 *pt, vector<Vector2*>& kPolygon)
{
	if(kPolygon.size() < 3)
		return false;

	int wn = 0;

	vector<Vector2 *>::iterator it;

	// loop through all edges of the polygon
	for (it=kPolygon.begin(); it<kPolygon.end()-1; it++)
	{
		if ((*(it))->y <= pt->y)
		{         
			if ((*(it+1))->y > pt->y) 
				if (IsLeft( *it, *(it+1), pt) > 0)
					++wn;
		}
		else
		{                       
			if ((*(it+1))->y <= pt->y)
				if (IsLeft( *it, *(it+1), pt) < 0)
					--wn;
		}
	}
	if (wn==0)
		return false;

	return true;
}


ZFAudioSystem::AmbientArea::AmbientArea()
{
	m_iSoundID = -1;
	m_fGain = 1.0f;
	m_fFadeTimer = -1;
};

ZFAudioSystem::AmbientArea::~AmbientArea()
{
	for(int i=0; i<m_kPolygon.size(); i++)
		delete m_kPolygon[i];
}


void ZFAudioSystem::SetMusic(const string& strMusic)
{
	m_strNextMusic = strMusic;
	m_bMusicChange = true;
}

void ZFAudioSystem::UpdateMusic()
{
	//want to change music
	if(m_bMusicChange)
	{	
		//is playing music?
		if(m_iMusicID != -1)
		{
			//fade gain until < 0 then remove it
			if(m_fMusicFade > 0)
			{
				float fDelta = Min(m_pkZeroFps->GetFrameTime(),0.1f) * 0.5;
				m_fMusicFade -=  fDelta;
								
				if(m_fMusicFade < 0.0)
					m_fMusicFade = 0.0;

			}
			else
			{
				StopAudio(m_iMusicID);
				m_iMusicID = -1;
				m_strCurrentMusic = "";
			}
		}
		//not playing music
		else
		{
			//dont want to change anymore
			m_bMusicChange = false;
		
			//have a new one to start?
			if(!m_strNextMusic.empty())
			{
				//cout<<"Changing music to:"<<m_strNextMusic<<endl;
			
				m_fMusicFade = 0;
				m_strCurrentMusic = m_strNextMusic;
				m_strNextMusic = "";
				m_iMusicID = PlayAudio(m_strCurrentMusic,Vector3(0,0,0), Vector3(0,0,1), ZFAUDIO_LOOP, 0);
			}			
		}
	}
	else
	{
		if(m_iMusicID != -1 && m_fMusicFade < 1)
		{								
			float fDelta = Min(m_pkZeroFps->GetFrameTime(),0.1f) * 0.5;
			m_fMusicFade += fDelta;
			if(m_fMusicFade > 1.0)
				m_fMusicFade = 1.0;
				
		}
	}

	if(m_iMusicID != -1)
		SetGain(m_iMusicID,m_fMusicFade*m_fMusicGain);
}