#include "audiosystem.h"
#include <stdlib.h>
 
ZFSound::ZFSound()
{
	m_uiIndex = 0;
	m_szFileName = NULL;
	m_kPos.Set(0,0,0);
	m_kVel.Set(0,0,0);
	m_bLoop=false;
	m_iSourceIndex=-1;
	m_iNrOfPlays=0;
	m_iPriority=0;
}

ZFSound::~ZFSound()
{
	if(m_szFileName != NULL)
	{
		delete[] m_szFileName;
		m_szFileName = NULL;
	}

	m_uiIndex = 0;
}

bool ZFSound::Create(string strName)
{
   // create buffer and set index
   alGetError(); /* clear */
   alGenBuffers(1, &m_uiIndex);
   if(alGetError() != AL_NO_ERROR) 
	   return false;

	// register filename
	if(m_szFileName != NULL)
		delete[] m_szFileName;

	m_szFileName = new char[strName.size()+1];
	strcpy(m_szFileName, strName.c_str());

	return true;
}

bool ZFSound::Load()
{
	ALsizei size=0, freq;
   ALenum format;
   ALvoid *data;
	ALboolean loop;   //we dont use this
	
	// Load file
   alutLoadWAVFile((ALbyte*)m_szFileName, &format, &data, &size, &freq, &loop);   
	if(size==0)
	{
	   cout<<"cold not load file " << m_szFileName << ", Loading Dummy" << endl;	
	   alutLoadWAVFile((ALbyte*)DUMMY_SOUND,&format,&data, &size, &freq,&loop);
   }

	// Fill buffer
	ALenum error;
	alBufferData(m_uiIndex, format, data, size, freq);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("ZFSound::Load() : alBufferData Failed!\n\n\n");
		alDeleteBuffers(1, &m_uiIndex);
		exit(-1);
	}

	return true;
}

void ZFSound::Start(int iSourceIndex, int iSourceName)
{
	m_iSourceIndex = iSourceIndex;

	Load();

  	alSourcei(iSourceName, AL_BUFFER, m_uiIndex);	
   alSourcef(iSourceName, AL_REFERENCE_DISTANCE, 3.5f);
	alSourcef(iSourceName, AL_GAIN, 1.0f);
	alSourcefv(iSourceName, AL_POSITION, &m_kPos[0]);	
	alSourcefv(iSourceName, AL_VELOCITY, &m_kVel[0]);	
   
   if(m_bLoop == true)
	   alSourcei(iSourceName, AL_LOOPING, 1);
	else
		alSourcei(iSourceName, AL_LOOPING, 0);

	alGetError(); // Clear Error Code

	alSourcePlay(iSourceName);	

	int error;
	if ((error = alGetError()) != AL_NO_ERROR)
		printf("Failed to play sound!\n");
}

ZFResource* Create__WavSound()
{
	return new ZFSound;
}

ZFAudioSystem::ZFAudioSystem() : ZFSubSystem("ZFAudioSystem") 
{
	GetSystem().Log_Create("audio");

	Register_Cmd("musicload",FID_MUSICLOAD);
	Register_Cmd("musicplay",FID_MUSICPLAY);
	Register_Cmd("musicstop",FID_MUSICSTOP);
	Register_Cmd("musicvolume",FID_MUSICVOLUME);
	Register_Cmd("musicbuffers",FID_MUSICBUFFERS);
}

ZFAudioSystem::~ZFAudioSystem()
{
	for(unsigned int i=0;i<m_kSources.size();i++)
		delete m_kSources[i];
	
	delete m_pkMusic;

	alutExit();
}

bool ZFAudioSystem::StartUp()	
{ 
	m_pkMusic = static_cast<OggMusic*>(g_ZFObjSys.GetObjectPtr("OggMusic"));

	Init();
	GenerateSources(MAX_NUM_SOURCES);
	return true; 
}

bool ZFAudioSystem::ShutDown() { return true; }

bool ZFAudioSystem::IsValid()  { return true; }

void ZFAudioSystem::GenerateSources(int iNrOfSources)
{
	for(int i=0;i<iNrOfSources;i++) 
	{
		SourceAlloc *temp=new SourceAlloc;	
		temp->m_bUsed=false;
		alGenSources(1, &temp->m_iSource);   
		m_kSources.push_back(temp);	
	}
}

void ZFAudioSystem::Init() 
{
   alutInit (NULL, 0); // init OpenAL

	alGetError(); // Clear Error Code

   // global settings
   alListenerf(AL_GAIN, 1.0f);

   alDopplerFactor(1.0f); // don't exaggerate doppler shift

   alDopplerVelocity(343); // using meters/second

	SetListnerPosition(Vector3(0,0,0),Vector3(0,1,0),Vector3(0,1,0));
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

int ZFAudioSystem::GetUnusedSource()
{
	for(unsigned int i=0;i<m_kSources.size();i++) 
		if(m_kSources[i]->m_bUsed==false)
			return i;

	return -1;
}

void ZFAudioSystem::AddSound(ZFSound* pkSound) 
{
	// Check if sound already exist.
	for(list<ZFSound*>::iterator it=m_akSounds.begin();it!=m_akSounds.end();it++)
		if( (*it) == pkSound)
			return;
	
	m_akSounds.push_back(pkSound);
}

void ZFAudioSystem::RemoveSound(ZFSound* pkSound) 
{
	if(pkSound==NULL)
		return;
	
	bool found=false;
	list<ZFSound*>::iterator it;
	for(it=m_akSounds.begin();it!=m_akSounds.end();it++)
	{	
		if( (*it) == pkSound)
			found=true;	
	}
	
	if(found==true)
	{
		if(pkSound->m_iSourceIndex!=-1)
		{
			int index = pkSound->m_iSourceIndex;
			alSourceStop(m_kSources[index]->m_iSource);				
			m_kSources[index]->m_bUsed=false;
			pkSound->m_iSourceIndex=-1;
		}
		pkSound->m_iNrOfPlays++;
		m_akSounds.remove(pkSound);

		printf("removing = %i\n", m_akSounds.size());
	}
}

void ZFAudioSystem::Update()
{
	m_pkMusic->Update();
	
	vector<ZFSound*> kPlay;
	vector<ZFSound*> kRemove;
	
	list<ZFSound*>::iterator it;
	for(it=m_akSounds.begin();it!=m_akSounds.end();it++) 
	{
		ZFSound* pkCurrent=(*it);
	
		switch(GetState(pkCurrent))
		{
		case -1:
			//Sound thats going to be played
			if(Hearable(pkCurrent))
				kPlay.push_back(pkCurrent);
			break;

		case AL_PLAYING:
			if(Hearable(pkCurrent))
			{
				int index = pkCurrent->m_iSourceIndex; // 0
				alSourcefv(m_kSources[index]->m_iSource, 
					AL_POSITION,&pkCurrent->m_kPos[0]);	
				alSourcefv(m_kSources[index]->m_iSource, 
					AL_VELOCITY,&pkCurrent->m_kVel[0]);	
				//sounds that cant be heard anymore
			} 
			else 
			{
				//if int aint looping remove it
				if(pkCurrent->m_bLoop==false)
					kRemove.push_back(pkCurrent);					
				//else stop it and dealocate the sound source
				else 
				{
					int index = pkCurrent->m_iSourceIndex;
					alSourceStop( m_kSources[index]->m_iSource );
					m_kSources[index]->m_bUsed=false;
					pkCurrent->m_iSourceIndex=-1;			
				}				
			}	
			break;

		case AL_STOPPED:
			if(pkCurrent->m_bLoop==false)
				kRemove.push_back(pkCurrent);
			break;
		}
	}
	
	unsigned int i;
	for(i=0;i<kRemove.size();i++)
		RemoveSound(kRemove[i]);	
		
	for(i=0;i<kPlay.size();i++)
	{
		int iSource=GetUnusedSource();
		if(iSource==-1)
			break;

		kPlay[i]->Start(iSource, m_kSources[iSource]->m_iSource);
	}
}

bool ZFAudioSystem::Hearable(ZFSound* pkSound)
{
	if( (pkSound->m_kPos-m_kPos).Length() < 100 ) 
		return true;
	else
		return false;
}

ALint ZFAudioSystem::GetState(ZFSound* pkSound)
{
	if(pkSound->m_iSourceIndex == -1)
		return -1;
	
	// alGetSourceiv ???
	ALint state;	
	int index = pkSound->m_iSourceIndex;
	alGetSourcei(m_kSources[index]->m_iSource,AL_SOURCE_STATE,&state);
	
	return state;
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