// zfaudiosystem.cpp: implementation of the ZFAudioSystem class.
//
//////////////////////////////////////////////////////////////////////

#include <AL/al.h>
#include <AL/alut.h>
#include "../../basic/zfvfs.h"
#include "../script_interfaces/si_audio.h"
#include "zfaudiosystem.h"

#define NO_SOURCE 999
#define AUDIO_CACH_SIZE 19021844 // ca. 18 Meg
				        
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//																			 //
// ZFSoundRes																 //
//																		     //
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

	// L�t OpenAL f�rst�ra bufferten
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
// �verlagrad funktion som anropas fr�n resursystemet via SetRes n�r ett nytt
// ljud skall laddas in fr�n disk.
///////////////////////////////////////////////////////////////////////////////
bool ZFSoundRes::Create(string strName)
{
	if(strName.empty())
		return false;

	// Anv�nd det virituella filsytemet f�r att kolla om filen finns och
	// spara undan s�kv�gen till filen i en medlems variabel
	ZFVFileSystem* pkFileSys;
	pkFileSys = static_cast<ZFVFileSystem*>(g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));		
	string strFull = pkFileSys->GetFullPath(strName.c_str());

	m_szFileName = new char[strFull.size()+1];
	strcpy(m_szFileName, strFull.c_str());
	
	return Load();
}


///////////////////////////////////////////////////////////////////////////////
// Ladda in en wav fil fr�n disk mha Alut.
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
// Unders�k storleken p� resursen.
///////////////////////////////////////////////////////////////////////////////
int ZFSoundRes::CalculateSize()
{
	return (int) m_uiSize;
}


///////////////////////////////////////////////////////////////////////////////
// H�mta OpenAL index f�r ljudbufferten. Annv�nds internt av ljudsystemet.
///////////////////////////////////////////////////////////////////////////////
ALuint ZFSoundRes::GetBufferIndexName()
{
	return m_uiBufferIndexName;
}


///////////////////////////////////////////////////////////////////////////////
// Skapa ett wav ljud. Annv�nds inte.
///////////////////////////////////////////////////////////////////////////////
ZFResource* Create__WavSound()
{
	return new ZFSoundRes;
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//																			 //
// ZFSoundInfo																 //
//																		     //
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

	m_pkResource = NULL;
	m_bLoopingNoLongerHearable = false;
}

ZFSoundInfo::~ZFSoundInfo()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//																			 //
// ZFAudioSystem														     //
//																		     //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ZFAudioSystem::ZFAudioSystem() : ZFSubSystem("ZFAudioSystem") 
{
	m_pkMusic = NULL;
	m_bIsValid = false;
	m_uiCurrentCachSize = 0;
}

ZFAudioSystem::~ZFAudioSystem()
{

}


///////////////////////////////////////////////////////////////////////////////
// Skapa ett nytt och l�gg till det till systemet.
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::StartSound(string strName, Vector3 pos, 
							   Vector3 dir, bool bLoop)
{
	ZFSoundInfo *pkSound = new ZFSoundInfo(strName.c_str(), pos, dir, bLoop);
	
	if(!InitSound(pkSound))
	{
		delete pkSound; // delete sound again
		return false;
	}
	
	// L�gg till ljudet till vektorn med ljud.
	m_kSoundList.push_back( pkSound );

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Leta reda p� det n�rmsta ljudet och ta bort det.
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::StopSound(string szName, Vector3 pos)
{
	ZFSoundInfo *pkSound = GetClosest(szName.c_str(), pos);

	if(pkSound != NULL)
	{
		DeleteSound(pkSound, true);

		// Ladda ur resursen om cachen �r fylld och om
		// resursen enbart anv�nds av det h�r ljudet.
		if(m_uiCurrentCachSize >= AUDIO_CACH_SIZE)
		{
			if(ResourceIsUnused(pkSound))
			{	
				UnLoadSound(string(pkSound->m_acFile));
			}
		}

		return true;
	}

	return false;
}


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
			
			m_uiCurrentCachSize += size; // �ka p� cachen

			printf("Loaded sound: %s (index:%i, %i bytes), cach size = %i\n", 
				strFileName.c_str(), 
				((ZFSoundRes*) pkResHandle->GetResourcePtr())->GetBufferIndexName(),
				size, m_uiCurrentCachSize );

			return true;
		}
	}
	else
	{
		// Already exist
		return true;
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// Ladda ur ett ljud (via resurssystemet).
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::UnLoadSound(string strFileName)
{
	map<string,ZFResourceHandle*>::iterator itRes;
	itRes = m_mkResHandles.find(strFileName);
	if(itRes != m_mkResHandles.end())
	{
		ZFResourceHandle* pkResHandle = itRes->second;

		if(pkResHandle->IsValid() == true)
		{
			int size = pkResHandle->GetResourcePtr()->CalculateSize();
				
			m_uiCurrentCachSize -= size; // minska ner cachen
			if(m_uiCurrentCachSize < 0)
				m_uiCurrentCachSize = 0;

			printf("UnLoading sound: %s (index:%i, %i bytes), cach size = %i\n", 
				strFileName.c_str(), 
				((ZFSoundRes*) pkResHandle->GetResourcePtr())->GetBufferIndexName(),
				size, m_uiCurrentCachSize );

			DeleteSoundsUsingResource((ZFSoundRes*)pkResHandle->GetResourcePtr());

			m_mkResHandles.erase(itRes);

			delete pkResHandle; // laddu ur resursen

			return true;
		}
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// Kolla hur m�nga ljud som f�r tillf�llet hanteras av systemet.
///////////////////////////////////////////////////////////////////////////////
unsigned int ZFAudioSystem::GetNumSounds()
{
	return m_kSoundList.size();
}


///////////////////////////////////////////////////////////////////////////////
// Kolla hur m�nga ljud som f�r tillf�llet spelas upp av systemet.
///////////////////////////////////////////////////////////////////////////////
unsigned int ZFAudioSystem::GetNumActiveChannels()
{
	unsigned int uiAntal = 0;

	list<ZFSoundInfo*>::iterator itSound = m_kSoundList.begin();
	for( ; itSound != m_kSoundList.end(); itSound++)  
	{
		if((*itSound)->m_uiSourceBufferName != NO_SOURCE)
			uiAntal++;
	}

	return uiAntal;
}


///////////////////////////////////////////////////////////////////////////////
// Starta upp systemet.
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::StartUp()
{
	m_pkMusic = static_cast<OggMusic*>(g_ZFObjSys.GetObjectPtr("OggMusic"));

	Register_Cmd("musicload",FID_MUSICLOAD);
	Register_Cmd("musicplay",FID_MUSICPLAY);
	Register_Cmd("musicstop",FID_MUSICSTOP);
	Register_Cmd("musicvolume",FID_MUSICVOLUME);
	Register_Cmd("musicbuffers",FID_MUSICBUFFERS);

	alutInit (0, NULL); 
	alGetError();
	alListenerf(AL_GAIN, 1.0f);
	alDopplerFactor(1.0f); 
	alDopplerVelocity(343); 

	SetListnerPosition(Vector3(0,0,0),Vector3(0,1,0),Vector3(0,1,0));

	ObjectManager* pkObjectMan = reinterpret_cast<ObjectManager*>(
		g_ZFObjSys.GetObjectPtr("ObjectManager"));

	ZFScriptSystem* pkScriptSys = reinterpret_cast<ZFScriptSystem*>(
		GetSystem().GetObjectPtr("ZFScriptSystem"));

	// Setup script interface.
	AudioLua::Init(this,pkObjectMan,pkScriptSys);

	m_bIsValid = true;

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// St�ng ner systemet.
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::ShutDown()
{
	if(m_pkMusic)
		delete m_pkMusic;

	// Remove resurce files
	map<string,ZFResourceHandle*>::iterator itRes = m_mkResHandles.begin();
	for( ; itRes != m_mkResHandles.end(); itRes++)
		if(itRes->second)
		{
			DeleteSoundsUsingResource((ZFSoundRes*)itRes->second->GetResourcePtr());
		 	delete itRes->second; // unload resource
		}

	// Destroy OpenAL (very, very, fucking importent)
	alutExit();

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Unders�k om systemet �r initierat.
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::IsValid()
{
	return m_bIsValid;
}


///////////////////////////////////////////////////////////////////////////////
// K�r kommandon fr�n konsolen.
///////////////////////////////////////////////////////////////////////////////
void ZFAudioSystem::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) 
	{
		case FID_MUSICLOAD:
			if(kCommand->m_kSplitCommand.size() > 1)
				m_pkMusic->LoadFile(kCommand->m_kSplitCommand[1]);
			break;
		
		case FID_MUSICPLAY:
			m_pkMusic->Play();
			break;

		case FID_MUSICSTOP:
			m_pkMusic->Stop();
			break;
		
		case FID_MUSICVOLUME:
			if(kCommand->m_kSplitCommand.size() > 1)
				m_pkMusic->SetVolume((float) 
					strtod( (kCommand->m_kSplitCommand[1]).c_str(),NULL));
			break;
			
		case FID_MUSICBUFFERS:
			{
				if(kCommand->m_kSplitCommand.size() > 1)
				{
					int iTemp = atoi((kCommand->m_kSplitCommand[1]).c_str());
					if(iTemp > 0)
					{
						m_pkMusic->Stop();
						delete m_pkMusic;
						m_pkMusic = new OggMusic(iTemp, 4096);
					}
				}
			}
			break;
	}
}


///////////////////////////////////////////////////////////////////////////////
// Uppdatera ljudsystemet. Anropas fr�n mainloopen.
///////////////////////////////////////////////////////////////////////////////
void ZFAudioSystem::Update()
{
	// Spela upp ogg music.
	m_pkMusic->Update();

	// Tempor�r vektor som fylls med det ljud som inte l�ngre kan h�ras
	// eller som har stannat.
	vector<ZFSoundInfo*> kStopped;

	// Tempor�r vektor med loop ljud som skall startas om.
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
				alSourcefv(pkSound->m_uiSourceBufferName, 
					AL_POSITION,&pkSound->m_kPos[0]);	
				alSourcefv(pkSound->m_uiSourceBufferName, 
					AL_VELOCITY,&pkSound->m_kDir[0]);
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

	unsigned int i;

	// Stoppa f�rdiga ljud.
	for( i=0; i<kStopped.size(); i++)
		Stop(kStopped[i]);

	// Start om loopade ljud.
	for( i=0; i<kRestart.size(); i++)
	{
		if(Restart(kRestart[i]) == false)
		{
			DeleteSound(kRestart[i], true);

			// Ladda ur resursen om cachen �r fylld och om
			// resursen enbart anv�nds av det h�r ljudet.
			if(m_uiCurrentCachSize >= AUDIO_CACH_SIZE)
			{
				if(ResourceIsUnused(kRestart[i]))
				{	
					UnLoadSound(string(kRestart[i]->m_acFile));
				}
			}
		}
	}

}


///////////////////////////////////////////////////////////////////////////////
// S�tt en ny position och orientering p� lysnaren.
///////////////////////////////////////////////////////////////////////////////
void ZFAudioSystem::SetListnerPosition(Vector3 kPos,Vector3 kHead,Vector3 kUp) 
{
	m_kPos=kPos;
	m_kHead=kHead;
	m_kUp=kUp;

	alListenerfv(AL_POSITION, &kPos[0]);
	
	float orientation[]={kHead.x,kHead.y,kHead.z, kUp.x,kUp.y,kUp.z};
	alListenerfv(AL_ORIENTATION, orientation);
}


///////////////////////////////////////////////////////////////////////////////
// Skriv ut eff felmeddelande i konsolf�nstret.
///////////////////////////////////////////////////////////////////////////////
void ZFAudioSystem::PrintError(ALenum error, char *szDesc)
{
	printf("%s Error: ", szDesc);

	switch(error)
	{
	case AL_INVALID_NAME:
		printf(" AL_INVALID_NAME\n");
		break;
	case AL_INVALID_ENUM:
		printf(" AL_INVALID_ENUM\n");
		break;
	case AL_INVALID_VALUE:
		printf(" AL_INVALID_VALUE\n");
		break;
	case AL_INVALID_OPERATION:
		printf(" AL_INVALID_OPERATION\n");
		break;
	case AL_OUT_OF_MEMORY:
		printf(" AL_OUT_OF_MEMORY\n");
		break;
	}
}



///////////////////////////////////////////////////////////////////////////////
// Returnerar ett resurshantag om ett s�dant finns eller skapar
// ett nytt och sparar ner det i en map om inget finns.
///////////////////////////////////////////////////////////////////////////////
ZFResourceHandle* ZFAudioSystem::GetResHandle(string strFileName)
{	
	// Check if resource handle exist.
	map<string,ZFResourceHandle*>::iterator itRes;
	itRes = m_mkResHandles.find(strFileName);
	if(itRes != m_mkResHandles.end())
		return itRes->second; // return if exist.

	// Add resource if not.
	ZFResourceHandle* pkNewRes = new ZFResourceHandle;
	m_mkResHandles.insert(map<string,ZFResourceHandle*>::value_type(
		strFileName, pkNewRes)); 

	return pkNewRes;
}


///////////////////////////////////////////////////////////////////////////////
// Unders�k om ett ljud �r g�r att h�ra.
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::Hearable(ZFSoundInfo* pkSound)
{
	if( (pkSound->m_kPos - m_kPos).Length() < 100 ) 
		return true;

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// Leta reda p� n�rmsta ljud med det namnet och den positionen
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


void ZFAudioSystem::DeleteSoundsUsingResource(ZFSoundRes* pkResource)
{
	vector<ZFSoundInfo*> kRemove;

	list<ZFSoundInfo*>::iterator it = m_kSoundList.begin();
	for( ; it != m_kSoundList.end(); it++)
	{
		ZFSoundInfo* pkSound = (*it);

		if(pkSound->m_pkResource == pkResource)
			kRemove.push_back(pkSound);
	}

	for(unsigned int i=0; i<kRemove.size(); i++)
		DeleteSound(kRemove[i], true);
}

///////////////////////////////////////////////////////////////////////////////
// Radera ljudk�lla (source) sammt eventuellt ljudbuffert.
// Tar bort ljudet ur systemet om andra argumetet �r satt till true.
///////////////////////////////////////////////////////////////////////////////
void ZFAudioSystem::DeleteSound(ZFSoundInfo *pkSound, bool bRemoveFromSystem)
{
	ALenum error;

	// Stoppa ljudet
	alGetError(); // clear
	alSourceStop(pkSound->m_uiSourceBufferName);
	if( (error = alGetError()) != AL_NO_ERROR)
	{
		ZFAudioSystem::PrintError(error, "Failed to stop sound before delete!"); 
		printf("name = %i\n", pkSound->m_uiSourceBufferName);
	}

	// Deallokera source pekaren
	alGetError(); // clear
	alDeleteSources(1, &pkSound->m_uiSourceBufferName);
	if( (error = alGetError()) != AL_NO_ERROR)
		ZFAudioSystem::PrintError(error, "Failed to delete source buffer!"); 

	pkSound->m_uiSourceBufferName = NO_SOURCE;

	// Ta bort ljudet ur systemet.
	if(bRemoveFromSystem)
	{
		m_kSoundList.remove( pkSound ) ;
		delete pkSound; // remove from system
	}
	else
		pkSound->m_pkResource = NULL;
}


///////////////////////////////////////////////////////////////////////////////
// Unders�ker om det inte finns n�got annat ljud i systemet som anv�nder
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
// F�rs�k spela upp ett ljud.
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

	// Set buffer
	alGetError();
	alSourcei(pkSound->m_uiSourceBufferName, AL_BUFFER, buffer );	
	if( (error = alGetError()) != AL_NO_ERROR)
	{
		PrintError(error, "ZFAudioSystem::Play, Failed to set buffer!");
		return false;
	}
	
	// Set reference distance
	alGetError();
	alSourcef(pkSound->m_uiSourceBufferName, AL_REFERENCE_DISTANCE, 3.5f);
	if( (error = alGetError()) != AL_NO_ERROR)
	{
		PrintError(error, "ZFAudioSystem::Play, Failed to set reference distance!");
		return false;
	}
	
	// Set gain
	alGetError();
	alSourcef(pkSound->m_uiSourceBufferName, AL_GAIN, 1.0f);
	if( (error = alGetError()) != AL_NO_ERROR)
	{
		PrintError(error, "ZFAudioSystem::Play, Failed to set gain!");
		return false;
	}
	
	// Set position
	alGetError();
	alSourcefv(pkSound->m_uiSourceBufferName, AL_POSITION, &pkSound->m_kPos[0]);	
	if( (error = alGetError()) != AL_NO_ERROR)
	{
		PrintError(error, "ZFAudioSystem::Play, Failed to set position!");
		return false;
	}
	
	// Set velocity
	alGetError();
	alSourcefv(pkSound->m_uiSourceBufferName, AL_VELOCITY, &pkSound->m_kDir[0]);
	if( (error = alGetError()) != AL_NO_ERROR)
	{
		PrintError(error, "ZFAudioSystem::Play, Failed to set velocity!");
		return false;
	}	

	// Set loop mode
	alGetError();
	alSourcei(pkSound->m_uiSourceBufferName, AL_LOOPING, pkSound->m_bLoop ? 1 : 0);
	if( (error = alGetError()) != AL_NO_ERROR)
	{
		PrintError(error, "ZFAudioSystem::Play, Failed to set loop mode!");
		return false;
	}

	// Play
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
// F�rs�k stoppa ett ljud. �r det ett vanligt ljud s� tas det ur systemet
// (men resursen beh�lls innladdad ifall fler ljud anv�nder den). �r det ett 
// loopat ljud s� beh�lls ljudet i systemet men ljudk�llan (source) f�rst�rs
// s� att andra ljud kan anv�ndas p� den lediga ljudkanalen.
///////////////////////////////////////////////////////////////////////////////
void ZFAudioSystem::Stop(ZFSoundInfo *pkSound)
{
	if(pkSound->m_uiSourceBufferName == NO_SOURCE)
		return; // already stoped

	printf("stopping sound\n");
	if(pkSound->m_bLoop)
		DeleteSound(pkSound, false);
	else
		DeleteSound(pkSound, true);

	// Ladda ur resursen om cachen �r fylld och om
	// resursen enbart anv�nds av det h�r ljudet.
	if(m_uiCurrentCachSize >= AUDIO_CACH_SIZE)
	{
		if(ResourceIsUnused(pkSound))
		{	
			UnLoadSound(string(pkSound->m_acFile));
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// Starta om ett ljud som redan finns i systemet.
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::Restart(ZFSoundInfo *pkSound)
{
	printf("restarting sound\n");
	
	if(!InitSound(pkSound))
		return false;
	
	if(pkSound->m_bLoop)
		pkSound->m_bLoopingNoLongerHearable = false;

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Ladda in ett nytt ljud om det beh�vs och generera en ny source buffer.
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::InitSound(ZFSoundInfo *pkSound)
{
	ZFResourceHandle* pkResHandle = GetResHandle(string(pkSound->m_acFile));

	// Ladda in om det beh�vs.
	if(pkResHandle->IsValid() == false)
		LoadSound(string(pkSound->m_acFile));

	// S�tt resurs pekaren.
	pkSound->m_pkResource = 
		reinterpret_cast<ZFSoundRes*>(pkResHandle->GetResourcePtr());

	// Generera en ny source buffer.
	ALenum error;
	alGetError();
	alGenSources(1, &pkSound->m_uiSourceBufferName);
	if( (error = alGetError()) != AL_NO_ERROR)
	{
		PrintError(error, "Failed to generate sound source!");
		return false;
	}

	return true;
}