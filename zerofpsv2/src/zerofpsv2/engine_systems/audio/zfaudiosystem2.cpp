// zfaudiosystem2.cpp: implementation of the ZFAudioSystem2 class.
//
//////////////////////////////////////////////////////////////////////

#include <al/al.h>
#include <al/alut.h>
#include "../../basic/zfvfs.h"
#include "zfaudiosystem2.h"

///////////////////////////////////////////////////////////////////////////////
// ZFSoundRes
///////////////////////////////////////////////////////////////////////////////

ZFSoundRes::ZFSoundRes()
{
	m_uiBufferIndexName = 0;
	m_szFileName = NULL;
}

ZFSoundRes::~ZFSoundRes()
{
	if(m_szFileName != NULL)
	{
		delete[] m_szFileName;
		m_szFileName = NULL;
	}

	if(m_uiBufferIndexName != 0)
	{
		alDeleteBuffers(1, &m_uiBufferIndexName);
	}
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
	
	bool bSuccess = Load();

	return true;
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

	// Fill buffer
	alBufferData(m_uiBufferIndexName, format, data, size, freq);
	if( alGetError() != AL_NO_ERROR)
	{
		printf("ZFSound::Load() : alBufferData Failed!\n\n\n");
		alDeleteBuffers(1, &m_uiBufferIndexName);
		return false;
	}

	//release the data
	alutUnloadWAV(format, data, size, freq);

	return true;
}

int ZFSoundRes::CalculateSize()
{
	return 4;
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
// SoundInfo
///////////////////////////////////////////////////////////////////////////////

 SoundInfo::SoundInfo()
{
	m_bLoop = false;
	m_kPos = Vector3(0,0,0);
	m_kDir = Vector3(0,0,1);
	m_uiSourceBufferName = 0;
	m_pkResource = NULL;
}

SoundInfo::~SoundInfo()
{
	
}

///////////////////////////////////////////////////////////////////////////////
// ZFAudioSystem2
///////////////////////////////////////////////////////////////////////////////

ZFAudioSystem2::ZFAudioSystem2() : ZFSubSystem("ZFAudioSystem2") 
{
	m_pkMusic = NULL;
}

ZFAudioSystem2::~ZFAudioSystem2()
{
//	if(m_pkMusic)
//		delete m_pkMusic;
}

bool ZFAudioSystem2::StartUp()
{
	m_pkMusic = static_cast<OggMusic*>(g_ZFObjSys.GetObjectPtr("OggMusic"));

   alutInit (0, NULL); 
	alGetError();
   alListenerf(AL_GAIN, 1.0f);
   alDopplerFactor(1.0f); 
   alDopplerVelocity(343); 

	SetListnerPosition(Vector3(0,0,0),Vector3(0,1,0),Vector3(0,1,0));

	GenerateSourcePool();

	return true;
}

bool ZFAudioSystem2::GenerateSourcePool()
{
	for(int i=0; i<SOURCE_POOL_SIZE; i++)
	{
		alGenSources(1, &m_akSourcePool[i].first);
		m_akSourcePool[i].second = false;
	}

	return true;
}

bool ZFAudioSystem2::ShutDown()
{
	return true;
}

bool ZFAudioSystem2::IsValid()
{
	return true;
}

void ZFAudioSystem2::RunCommand(int cmdid, const CmdArgument* kCommand)
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

void ZFAudioSystem2::Update()
{
	// Spela upp ogg music.
	m_pkMusic->Update();

	vector<SoundInfo*> kRemove;

	list<SoundInfo*>::iterator itSound = m_kActiveSounds.begin();
	for( ; itSound != m_kActiveSounds.end(); itSound++)  
	{
		SoundInfo* pkSound = (*itSound);

		// Kolla vilken status har ljudet.
		ALint iSourceState;
		alGetSourcei(pkSound->m_uiSourceBufferName, AL_SOURCE_STATE, &iSourceState);

		switch(iSourceState)
		{
		case AL_INITIAL: // Initierat.

			//
			// Starta ljudet.
			//
			if(Hearable( pkSound ))
			{
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

				alSourcei(pkSound->m_uiSourceBufferName, 
					AL_BUFFER, buffer );	
				alSourcef(pkSound->m_uiSourceBufferName, 
					AL_REFERENCE_DISTANCE, 3.5f);
				alSourcef(pkSound->m_uiSourceBufferName, 
					AL_GAIN, 1.0f);
				alSourcefv(pkSound->m_uiSourceBufferName, 
					AL_POSITION, &pkSound->m_kPos[0]);	
				alSourcefv(pkSound->m_uiSourceBufferName, 
					AL_VELOCITY, &pkSound->m_kDir[0]);
				alSourcei(pkSound->m_uiSourceBufferName, 
					AL_LOOPING, pkSound->m_bLoop ? 1 : 0);

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
			// Ändra rikting och velocity.
			//
			if(Hearable( pkSound ))
			{
				alSourcefv(pkSound->m_uiSourceBufferName, 
					AL_POSITION,&pkSound->m_kPos[0]);	
				alSourcefv(pkSound->m_uiSourceBufferName, 
					AL_VELOCITY,&pkSound->m_kDir[0]);	
			}
			else
			{
				if(pkSound->m_bLoop == false)
					kRemove.push_back( pkSound );
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
		SoundInfo* pkSound = kRemove[i];

		for(int j=0; j<SOURCE_POOL_SIZE; j++)
		{
			if(m_akSourcePool[j].first == pkSound->m_uiSourceBufferName) 
			{
				m_akSourcePool[j].second = false;
				alSourceStop(pkSound->m_uiSourceBufferName);
				pkSound->m_uiSourceBufferName = 0;
				m_kActiveSounds.remove( pkSound ) ;
				delete pkSound; // radera
				break;
			}
		}
	}
}

void ZFAudioSystem2::SetListnerPosition(Vector3 kPos,Vector3 kHead,Vector3 kUp) 
{
	m_kPos=kPos;
	m_kHead=kHead;
	m_kUp=kUp;

	alListenerfv(AL_POSITION, &kPos[0]);
	
	float orientation[]={kHead.x,kHead.y,kHead.z, kUp.x,kUp.y,kUp.z};
	alListenerfv(AL_ORIENTATION, orientation);
}

// Det är OK att skapa ett lokalt SoundInfo objekt och skicka in till funktionen.
// Funktioner skapar alltid en egen kopia på objektet som den tar hand om själv.
bool ZFAudioSystem2::ActivateSound(SoundInfo kSound)
{
	SoundInfo *pkSound = new SoundInfo;
	memcpy(pkSound, &kSound, sizeof(SoundInfo));

	if(pkSound == NULL || pkSound->m_szFile == NULL)
	{
		printf("ZFAudioSystem2::ActivateSound: Bad argument.\n");
		return false;
	}

	// Hämta ett resurshantag.
	ZFResourceHandle* pkResHandle = GetResHandle(string(pkSound->m_szFile));

	// Är resursen ej inladdad?
	if(pkResHandle->IsValid() == false)
	{
		// Försök ladda in ljudet från disk via resurs systemet.
		if(pkResHandle->SetRes(pkSound->m_szFile) == false)
		{
			printf("ZFAudioSystem2::ActivateSound: SetRes failed!\n");
			return false;
		}
	}

	// Sätt resurspekaren.
	pkSound->m_pkResource = (ZFSoundRes*) pkResHandle->GetResourcePtr();

	// Hitta en ledigt ljudkanal.
	for(int i=0; i<SOURCE_POOL_SIZE; i++)
	{
		if(m_akSourcePool[i].second == false) // ledig kanal
		{
			// Markera att kanalen är upptagen
			m_akSourcePool[i].second = true;

			// Markera vilken source name index ljudet skall ha.
			pkSound->m_uiSourceBufferName = m_akSourcePool[i].first;

			break;
		}
	}

	// Lägg till ljudet till vektorn med aktiva ljud.
	m_kActiveSounds.push_back( pkSound );

	// Flagga att ljudet är initierat på nytt.
	alSourcei( pkSound->m_uiSourceBufferName, AL_SOURCE_STATE, AL_INITIAL );

	return true;
}

bool ZFAudioSystem2::StopSound(SoundInfo kSound)
{
	return true;
}

//
// Returnerar ett resurshantag om ett sådant finnes eller skapar
// ett nytt och sparar ner i en map om inget finnes.
//
ZFResourceHandle* ZFAudioSystem2::GetResHandle(string strFileName)
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

bool ZFAudioSystem2::Hearable(SoundInfo* pkSound)
{
	if( (pkSound->m_kPos-m_kPos).Length() < 100 ) 
		return true;
	else
		return false;
}

unsigned int ZFAudioSystem2::GetNumActiveSounds()
{
	return m_kActiveSounds.size();
}
