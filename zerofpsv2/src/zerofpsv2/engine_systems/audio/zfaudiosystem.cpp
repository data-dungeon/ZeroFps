// zfaudiosystem.cpp: implementation of the ZFAudioSystem class.
//
//////////////////////////////////////////////////////////////////////

#include <AL/al.h>
#include <AL/alut.h>
#include "../../basic/zfvfs.h"
#include "../script_interfaces/si_audio.h"
#include "zfaudiosystem.h"

///////////////////////////////////////////////////////////////////////////////
// ZFSoundRes
///////////////////////////////////////////////////////////////////////////////

ZFSoundRes::ZFSoundRes()
{
	m_uiBufferIndexName = 0;
	m_szFileName = NULL;
	m_uiSize = 0;
}

ZFSoundRes::~ZFSoundRes()
{
	printf("ZFSoundRes::~ZFSoundRes()\n");

	if(m_szFileName != NULL)
	{
		delete[] m_szFileName;
		m_szFileName = NULL;
	}

	// Låt OpenAL förstöra bufferten
	if(m_uiBufferIndexName != 0)
		alDeleteBuffers(1, &m_uiBufferIndexName);
}

bool ZFSoundRes::Create(string strName)
{
	if(strName.empty())
		return false;

	// Använd det virituella filsytemet för att kolla om filen finns och
	// spara undan sökvägen till filen i en medlems variabel
	ZFVFileSystem* pkFileSys;
	pkFileSys = static_cast<ZFVFileSystem*>(g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));		
	string strFull = pkFileSys->GetFullPath(strName.c_str());

	m_szFileName = new char[strFull.size()+1];
	strcpy(m_szFileName, strFull.c_str());
	
	return Load();
}

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
	else
	{
		printf("Loading sound %s\n", m_szFileName);
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

int ZFSoundRes::CalculateSize()
{
	return (int) m_uiSize;
}

ALuint ZFSoundRes::GetBufferIndexName()
{
	return m_uiBufferIndexName;
}

ZFResource* Create__WavSound()
{
	return new ZFSoundRes;
}

///////////////////////////////////////////////////////////////////////////////
// ZFSoundInfo
///////////////////////////////////////////////////////////////////////////////

ZFSoundInfo::ZFSoundInfo()
{
	m_bLoop = false;
	m_kPos = Vector3(0,0,0);
	m_kDir = Vector3(0,0,1);
	m_uiSourceBufferName = 0;
	m_pkResource = NULL;
	m_bLoopingNoLongerHearable = false;
}

ZFSoundInfo::ZFSoundInfo(const char* c_szFile, Vector3 pos, Vector3 dir, bool bLoop)
{
	strcpy(m_acFile, c_szFile);
	m_bLoop = bLoop;
	m_kPos = pos;
	m_kDir = dir;

	m_uiSourceBufferName = 0;
	m_pkResource = NULL;
	m_bLoopingNoLongerHearable = false;
}

ZFSoundInfo::~ZFSoundInfo()
{
/*	if(m_pkResource != NULL)
	{
		delete m_pkResource;
		m_pkResource = NULL;
	}*/
}

///////////////////////////////////////////////////////////////////////////////
// ZFAudioSystem
///////////////////////////////////////////////////////////////////////////////

ZFAudioSystem::ZFAudioSystem() : ZFSubSystem("ZFAudioSystem") 
{
	m_pkMusic = NULL;
	m_bIsValid = false;

	m_pkSourcePool = NULL;
}

ZFAudioSystem::~ZFAudioSystem()
{

}

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

	if(!GenerateSourcePool())
	{
		printf("ZFSoundRes::StartUp(), Failed to generate source pool! Reduce number of sources\n");
		return false;
	}

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

bool ZFAudioSystem::GenerateSourcePool()
{
	m_uiSourcePoolSize = 15; // Vi antar att vi åtminstånde har 16 kanaler, vilket
									 // ger oss 15 eftersom OggMusic använder en.

	m_pkSourcePool = new SOURCE_POOL[m_uiSourcePoolSize];

	alGetError(); // clear

	printf("---- Source names ----\n");

	for(unsigned i=0; i<m_uiSourcePoolSize; i++)
	{
		alGenSources(1, &m_pkSourcePool[i].first);

		if( alGetError() != AL_NO_ERROR)
			return false;

		m_pkSourcePool[i].second = false;

		printf("%i\n", (int) m_pkSourcePool[i].first);
	}

	printf("----------------\n");

	return true;
}

bool ZFAudioSystem::ShutDown()
{
	if(m_pkMusic)
		delete m_pkMusic;

	// Remove resurce files
	map<string,ZFResourceHandle*>::iterator itRes = m_mkResHandles.begin();
	for( ; itRes != m_mkResHandles.end(); itRes++)
		if(itRes->second)
		 	delete itRes->second;

	// Remove all sounds.
	list<ZFSoundInfo*>::iterator itSound = m_kActiveSounds.begin();
	for( ; itSound != m_kActiveSounds.end(); itSound++)
		delete (*itSound);

	delete[] m_pkSourcePool;

	// Destroy OpenAL (very, very, very, very, fuckig importent!)
	alutExit();

	return true;
}

bool ZFAudioSystem::IsValid()
{
	return m_bIsValid;
}

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

void ZFAudioSystem::SetListnerPosition(Vector3 kPos,Vector3 kHead,Vector3 kUp) 
{
	m_kPos=kPos;
	m_kHead=kHead;
	m_kUp=kUp;

	alListenerfv(AL_POSITION, &kPos[0]);
	
	float orientation[]={kHead.x,kHead.y,kHead.z, kUp.x,kUp.y,kUp.z};
	alListenerfv(AL_ORIENTATION, orientation);
}

/*
	Det är meningen att man skall skapa ett lokalt ZFSoundInfo objekt och skicka
	in till funktionen. Funktionen skapar alltid en egen kopia på objektet som 
	den tar hand om själv.
*/
bool ZFAudioSystem::StartSound(ZFSoundInfo kSound)
{
	ZFSoundInfo *pkSound = new ZFSoundInfo;
	memcpy(pkSound, &kSound, sizeof(ZFSoundInfo));

	if(pkSound == NULL || pkSound->m_acFile == NULL)
	{
		printf("ZFAudioSystem::ActivateSound: Bad argument.\n");
		return false;
	}

	// Ladda ljudet om det inte redan finns.
	LoadRes(pkSound);

	// Hitta en ledigt ljudkanal.
	int channel = GetFreeChannel();
	if(channel == -1)
	{
		printf("Failed to find free channel for sound\n");
		return false;
	}

	m_pkSourcePool[channel].second = true; // markera att kanalen är upptagen
	pkSound->m_uiSourceBufferName = m_pkSourcePool[channel].first; // registrera vilken ljudkanal ljudet spelar på

	// Flagga att ljudet är initierat på nytt.
	alSourceRewind( pkSound->m_uiSourceBufferName );

	// Lägg till ljudet till vektorn med aktiva ljud.
	m_kActiveSounds.push_back( pkSound );

	return true;
}

//
// Ta bort det ljud som ligger närmast [kSound] och som 
// heter samma som [kSound]. Avståndet mellan det funna ljudet
// och [kSound] bestäms av parametern fMaxSearchRange.
//
bool ZFAudioSystem::RemoveSound(ZFSoundInfo kSound, float fMaxSearchRange)
{
	ZFSoundInfo* pkRemoveSound = NULL;

	float fClosestDist = 100000.0f;

	// Leta reda på det närmsta ljudet med det namnet
	list<ZFSoundInfo*>::iterator itSound = m_kActiveSounds.begin();
	for( ; itSound != m_kActiveSounds.end(); itSound++)  
	{
		ZFSoundInfo* pkSound = (*itSound);

		if(strcmp(pkSound->m_acFile, kSound.m_acFile) == 0)
		{
			float fDistance = (kSound.m_kPos - pkSound->m_kPos).LengthSqr();

			if(fDistance < fClosestDist)
			{
				pkRemoveSound = pkSound;
				fClosestDist = fDistance;
			}
		}
	}

	// Stanna och ladda ur ljudet om det är innom godkännt avstånd.
	if(pkRemoveSound != NULL && sqrt(fClosestDist) < fMaxSearchRange)
	{
		for(unsigned int j=0; j<m_uiSourcePoolSize; j++)
		{
			if(m_pkSourcePool[j].first == pkRemoveSound->m_uiSourceBufferName) 
			{
				// Stoppa ljudet
				alSourceStop(pkRemoveSound->m_uiSourceBufferName);
				
				// Frigör poolen
				m_pkSourcePool[j].second = false;
				pkRemoveSound->m_uiSourceBufferName = 0;
				
				printf("removing\n");

				// Minska antalet laddade resurser och ladda ur resursen
				// om vi inga ljud använder den.
				if( ChangeResCounter(string(pkRemoveSound->m_acFile),-1) == 0)
				{
					UnLoadRes( pkRemoveSound );
				}

				// Ta bort ljudet ur listan.
				m_kActiveSounds.remove( pkRemoveSound ) ;
				delete pkRemoveSound; // radera	
				pkRemoveSound = NULL;

				return true;
			}
		}
	}

	return false;
}

void ZFAudioSystem::RemoveAllSounds()
{
	vector<ZFSoundInfo*> kRemoveArray;

	// Radera alla ljud
	list<ZFSoundInfo*>::iterator itSound = m_kActiveSounds.begin();
	for( ; itSound != m_kActiveSounds.end(); itSound++)  
		delete (*itSound); // radera

	m_kActiveSounds.clear();

	// Frigör poolen
	for(unsigned int i=0; i<m_uiSourcePoolSize; i++)
		m_pkSourcePool[i].second = false;
}

//
// Returnerar ett resurshantag om ett sådant finnes eller skapar
// ett nytt och sparar ner i en map om inget finnes.
//
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

//
// Öka på eller minska antalet aktiva resurser
//
unsigned short ZFAudioSystem::ChangeResCounter(string strFileName, 
															unsigned short modification)
{
	unsigned short num;

	// Check if resource handle exist.
	map<string,unsigned short>::iterator itCounter;
	itCounter = m_mkResHandleCounter.find(strFileName);

	if(itCounter != m_mkResHandleCounter.end())
	{
		// exist, increase..
		unsigned short number = itCounter->second;

		number+=modification;
		
		if(number <= 0)
		{
			m_mkResHandleCounter.erase(itCounter);
			num = 0;
		}
		else
		{
			num = itCounter->second = number;
		}
	}
	else
	{
		// do not exist, add 1
		m_mkResHandleCounter.insert(map<string,int>::value_type(
			strFileName, 1)); 
		num = 1;
	}
	
	return num;
}

bool ZFAudioSystem::Hearable(ZFSoundInfo* pkSound)
{
	if( (pkSound->m_kPos-m_kPos).Length() < 100 ) 
		return true;
	else
		return false;
}

unsigned int ZFAudioSystem::GetNumActiveSounds()
{
	return m_kActiveSounds.size();
}

unsigned int ZFAudioSystem::GetNumActiveChannels()
{
	unsigned int uiAntal = 0;

	for(unsigned int i=0; i<m_uiSourcePoolSize; i++)
		if(m_pkSourcePool[i].second == true)
			uiAntal++;

	return uiAntal;
}

bool ZFAudioSystem::RestartLoopSound(ZFSoundInfo *pkSound)
{
	// Ladda in ljudet på nytt
	if(pkSound->m_pkResource == NULL)
		LoadRes(pkSound);

	bool bFoundChannel = false;

	// Hitta en ledigt ljudkanal.
	int channel = GetFreeChannel();
	if(channel == -1)
		return false;

	m_pkSourcePool[channel].second = true;
	pkSound->m_uiSourceBufferName = m_pkSourcePool[channel].first;

	// Flagga att ljudet är initierat på nytt.
	alSourceRewind( pkSound->m_uiSourceBufferName );

	return bFoundChannel;
}

bool ZFAudioSystem::LoadRes(ZFSoundInfo* pkSound)
{
	// Hämta ett resurshantag.
	ZFResourceHandle* pkResHandle = GetResHandle(string(pkSound->m_acFile));

	// Öka på antalet laddade resurser
	ChangeResCounter(string(pkSound->m_acFile),1);

	// Är resursen ej inladdad?
	if(pkResHandle->IsValid() == false)
	{
		// Försök ladda in ljudet från disk via resurs systemet.
		if(pkResHandle->SetRes(pkSound->m_acFile) == false)
		{
			printf("ZFAudioSystem::ActivateSound: SetRes failed!\n");
			return false;
		}
		else
		{
			printf("Sound %s loaded (%i bytes)\n", pkSound->m_acFile, 
				(ZFSoundRes*) pkResHandle->GetResourcePtr()->CalculateSize() );
		}
	}

	// Sätt resurspekaren.
	pkSound->m_pkResource = (ZFSoundRes*) pkResHandle->GetResourcePtr();

	return true;
}

bool ZFAudioSystem::UnLoadRes(ZFSoundInfo* pkSound)
{
	// Check if resource handle exist.
	map<string,ZFResourceHandle*>::iterator itRes;
	itRes = m_mkResHandles.find(pkSound->m_acFile);
	if(itRes != m_mkResHandles.end())
	{
		delete itRes->second;
		m_mkResHandles.erase( itRes );
		return true;
	}

	return false;
}

void ZFAudioSystem::Update()
{
	//printf("size = %i\n", m_kActiveSounds.size());

	//
	// Spela upp ogg music.
	//
	m_pkMusic->Update();

	// Temporär vektor som fylls med det ljud som inte längre kan höras
	// eller som har stannat.
	vector<ZFSoundInfo*> kRemove;

	list<ZFSoundInfo*>::iterator itSound = m_kActiveSounds.begin();
	for( ; itSound != m_kActiveSounds.end(); itSound++)  
	{
		ZFSoundInfo* pkSound = (*itSound);

		bool bHearable = Hearable( pkSound );

		// Aktivera ljudet på nytt om det är ett loopat ljud och lyssnaren
		// har kommit såpass nära att han kan höra det.
		if(pkSound->m_bLoopingNoLongerHearable && bHearable)
		{
			pkSound->m_bLoopingNoLongerHearable = false;
			RestartLoopSound(pkSound); // Statusen på ljudet ändras 
		}

		// Kolla vilken status har ljudet.
		ALint iSourceState;
		alGetSourcei(pkSound->m_uiSourceBufferName, AL_SOURCE_STATE, &iSourceState);

		switch(iSourceState)
		{
		case AL_INITIAL: // Initierat.

			//
			// Starta ljudet.
			//
			if(bHearable)
			{
				printf("mmmm\n");

				if(pkSound->m_pkResource == NULL)
				{
					printf("pkSound->m_pkResource == NULL!\n");
					return;
				}

				unsigned int buffer = pkSound->m_pkResource->GetBufferIndexName();

				if(buffer == 0)
				{
					printf("buffer == 0!\n");
					return;
				}

				alSourcei(pkSound->m_uiSourceBufferName, AL_BUFFER, buffer );	
				alSourcef(pkSound->m_uiSourceBufferName, AL_REFERENCE_DISTANCE, 3.5f);
				alSourcef(pkSound->m_uiSourceBufferName, AL_GAIN, 1.0f);
				alSourcefv(pkSound->m_uiSourceBufferName, AL_POSITION, &pkSound->m_kPos[0]);	
				alSourcefv(pkSound->m_uiSourceBufferName, AL_VELOCITY, &pkSound->m_kDir[0]);
				alSourcei(pkSound->m_uiSourceBufferName, AL_LOOPING, pkSound->m_bLoop ? 1 : 0);

				alGetError(); // Clear Error Code

				alSourcePlay(pkSound->m_uiSourceBufferName);

				if( alGetError() != AL_NO_ERROR)
				{
					printf("ZFSoundRes::Load(), Failed to call alSourcePlay!\n");
					return;
				}
			}

			break;

		case AL_PLAYING: // Spelar.

			//
			// Ändra rikting och velocity om ljudets position har ändrats.
			//
			if(bHearable)
			{
				alSourcefv(pkSound->m_uiSourceBufferName, 
					AL_POSITION,&pkSound->m_kPos[0]);	
				alSourcefv(pkSound->m_uiSourceBufferName, 
					AL_VELOCITY,&pkSound->m_kDir[0]);
				printf("playing\n");
			}
			else
			{
				// Lägg till ljudet till erase vektorn om det inte längre hörs.
				kRemove.push_back( pkSound );
				printf("Stopping sound that no longer can be heard!\n");
			}

			break;

		case AL_STOPPED: // Stoppat.

			if(pkSound->m_bLoop == false)
				kRemove.push_back( pkSound );

			break;
		}
	}

	//
	// Stoppa färdiga ljud.
	//
	for(unsigned int i=0; i<kRemove.size(); i++)
	{
		ZFSoundInfo* pkSound = kRemove[i];

		for(unsigned int j=0; j<m_uiSourcePoolSize; j++)
		{
			if(m_pkSourcePool[j].first == pkSound->m_uiSourceBufferName) 
			{
				// Stoppa ljudet
				alSourceStop(pkSound->m_uiSourceBufferName);
				
				// Frigör poolen
				m_pkSourcePool[j].second = false;
				
				// Ta bort ljudet om det inte loopar. Ifall det loopar så 
				// behåller vi ljudet men laddar ur resursen och flaggar att 
				// det inte längre kan höras, så att vi sedan kan kolla det 
				// och aktivera det pånytt om lyssnaren skulle komma såpass 
				// nära att ljudet pånytt kan höras.
				if(pkSound->m_bLoop == false)
				{
					m_kActiveSounds.remove( pkSound ) ;
					delete pkSound; // radera	
				}
				else
				{
					pkSound->m_bLoopingNoLongerHearable = true;
					
					// Ladda ur resursen.
					delete pkSound->m_pkResource;
					pkSound->m_pkResource = NULL;
				}
				break;
			}
		}
	}

	
}

int ZFAudioSystem::GetFreeChannel()
{
	// Hitta en ledigt ljudkanal.
	for(unsigned int i=0; i<m_uiSourcePoolSize; i++)
		if(m_pkSourcePool[i].second == false) // ledig kanal
			return i;

	return -1;
}
