// zfaudiosystem.cpp: implementation of the ZFAudioSystem class.
//
//////////////////////////////////////////////////////////////////////

#include <AL/al.h>
#include "AL/alc.h"
#include <AL/alut.h>
#include "../../basic/zfvfs.h"
#include "../script_interfaces/si_audio.h"
#include "zfaudiosystem.h"

#define NO_SOURCE 999

Vector3 ZFAudioSystem::m_kPos = Vector3(0,0,0);

				        
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

	m_pkResource = NULL;
	m_bLoopingNoLongerHearable = false;
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
	m_pkMusic = NULL;
	m_bIsValid = false;
	m_uiCurrentCachSize = 0;
	m_uiMaxCachSize = uiMaxCachSize;
	m_fReferenceDistance = 1.0f;
	m_iEnableSound = 1;

	RegisterVariable("r_enablesound",&m_iEnableSound,CSYS_INT);
	RegisterVariable("r_ReferenceDistance",&m_fReferenceDistance,CSYS_FLOAT);
}

ZFAudioSystem::~ZFAudioSystem()
{

}


///////////////////////////////////////////////////////////////////////////////
// Skapa ett nytt och lägg till det till systemet.
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::StartSound(string strName, Vector3 pos, 
							   Vector3 dir, bool bLoop)
{
	ZFSoundInfo *pkSound = new ZFSoundInfo(strName.c_str(), pos, dir, bLoop);
	
	if(!InitSound(pkSound))
	{
		delete pkSound; // delete sound again.
		printf("Failed to start sound %s\n", strName.c_str());
		return false;
	}
	
	// Lägg till ljudet till vektorn med ljud.
	m_kSoundList.push_back( pkSound );

	//printf("Starting sound (priority: %i)\n", GetResHandlePriority(strName));

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Leta reda på det närmsta ljudet och ta bort det.
///////////////////////////////////////////////////////////////////////////////
bool ZFAudioSystem::StopSound(string strName, Vector3 pos)
{
	ZFSoundInfo *pkSound = GetClosest(strName.c_str(), pos);

	if(pkSound != NULL)
	{
		if(DeleteSound(pkSound, true))
		{
		//	printf("Stoping sound %s\n", strName.c_str());
			return true;
		}
	}

//	printf("Failed to stop sound %s\n", strName.c_str());

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
	m_pkMusic = static_cast<OggMusic*>(g_ZFObjSys.GetObjectPtr("OggMusic"));
	m_pkTreadInfo = new THREAD_INFO;

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

	m_fVolume = 1.0f;

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
	if( OggMusic::m_pkThread != NULL)
		SDL_KillThread(m_pkMusic->m_pkThread);

	if(m_pkMusic)
		delete m_pkMusic;

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

bool ZFAudioSystem::SetVolume(float fVolume)
{
	if((fVolume<=1.0) && (fVolume>0.0))
	{
		m_fVolume = fVolume;
		return true;
	}
	return false;
};


///////////////////////////////////////////////////////////////////////////////
// Uppdatera ljudsystemet. Anropas från mainloopen.
///////////////////////////////////////////////////////////////////////////////
void ZFAudioSystem::Update()
{
/*	// Kör igång mainloopen för musiktråden
	if( OggMusic::m_pkThread == NULL)
	{
		m_pkMusic->m_pkThread = SDL_CreateThread(OggMusic::ThreadMain, m_pkMusic);
	}
*/
	m_pkMusic->Update(m_kPos);

	if(m_iEnableSound == 0)
		return;

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
				alSourcefv(pkSound->m_uiSourceBufferName, 
					AL_POSITION,&pkSound->m_kPos[0]);	
				alSourcefv(pkSound->m_uiSourceBufferName, 
					AL_VELOCITY,&pkSound->m_kDir[0]);
				alSourcef(pkSound->m_uiSourceBufferName, 
					AL_GAIN, m_fVolume);
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

	// Stoppa färdiga ljud.
	for( i=0; i<kStopped.size(); i++)
		Stop(kStopped[i]);

	// Start om loopade ljud.
	for( i=0; i<kRestart.size(); i++)
	{
		if(Restart(kRestart[i]) == false)
		{
			DeleteSound(kRestart[i], true);
		}
	}

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
	ALenum error;

	// Stoppa ljudet.
	alGetError(); // clear
	alSourceStop(pkSound->m_uiSourceBufferName);
	if( (error = alGetError()) != AL_NO_ERROR)
	{
		ZFAudioSystem::PrintError(error, "Failed to stop sound before delete!"); 
		printf("name = %i\n", pkSound->m_uiSourceBufferName);
	}

	// Deallokera source pekaren.
	alGetError(); // clear
	alDeleteSources(1, &pkSound->m_uiSourceBufferName);
	if( (error = alGetError()) != AL_NO_ERROR)
		ZFAudioSystem::PrintError(error, "Failed to delete source buffer!"); 

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
	alSourcef(pkSound->m_uiSourceBufferName, AL_GAIN, 1.0f);
	if( (error = alGetError()) != AL_NO_ERROR)
	{
		PrintError(error, "ZFAudioSystem::Play, Failed to set gain!");
		return false;
	}
	
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
	printf("restarting sound\n");
	
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

	// Generera en ny source buffer.
	ALenum error;
	alGetError();
	alGenSources(1, &pkSound->m_uiSourceBufferName);
	if( (error = alGetError()) != AL_NO_ERROR)
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
			return InitSound(pkSound);
		}

		PrintError(error, "Failed to generate sound source!\n");
			
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Leta rätt på det närmsta ljudet med det namnet och flytta på det till ny postion
///////////////////////////////////////////////////////////////////////////////

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
	}

	return false;
}
