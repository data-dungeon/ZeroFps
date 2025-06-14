#include "openalsystem.h"
#include <stdlib.h>

Sound::Sound()
{
	m_acFile="";
	m_kPos.Set(0,0,0);
	m_kVel.Set(0,0,0);
	m_bLoop=false;
	m_iSource=-1;
	m_iNrOfPlays=0;

}


OpenAlSystem::OpenAlSystem()  
: ZFObject("OpenAlSystem") {
	sbm= static_cast<SoundBufferManager*>(g_ZFObjSys.GetObjectPtr("SoundBufferManager"));

	Init();
	GenerateSources(10);
	
	//Gubb was here
	g_ZFObjSys.Register_Cmd("musicload",FID_MUSICLOAD,this);
	g_ZFObjSys.Register_Cmd("musicplay",FID_MUSICPLAY,this);
	g_ZFObjSys.Register_Cmd("musicstop",FID_MUSICSTOP,this);
	g_ZFObjSys.Register_Cmd("musicvolume",FID_MUSICVOLUME,this);
	g_ZFObjSys.Register_Cmd("musicbuffers",FID_MUSICBUFFERS,this);
}

OpenAlSystem::~OpenAlSystem()
{
	for(unsigned int i=0;i<m_kSources.size();i++) {
		delete m_kSources[i];
	}
	
	//Gubb was here
	delete m_pkMusic;
}


void OpenAlSystem::GenerateSources(int m_iNrOfSources)
{
	for(int i=0;i<m_iNrOfSources;i++) {
		SourceAlloc *temp=new SourceAlloc;	
		temp->m_bUsed=false;
		alGenSources(1, &temp->m_iSource);   
//		cout<<"BLUB:"<<temp->m_iSource<<endl;
		
		m_kSources.push_back(temp);			
	}
}

void OpenAlSystem::Init() 
{
   
   alutInit (NULL, 0); // init OpenAL

   // global settings
   alListenerf(AL_GAIN, 1.0);
   alDopplerFactor(1.0); // don't exaggerate doppler shift
   alDopplerVelocity(343); // using meters/second

	SetListnerPosition(Vector3(0,0,0),Vector3(0,0,1),Vector3(0,1,0));
	//Gubb was here
	m_pkMusic = new OggMusic(24,4096);
}


void OpenAlSystem::SetListnerPosition(Vector3 kPos,Vector3 kHead,Vector3 kUp) 
{
	m_kPos=kPos;
	m_kHead=kHead;
	m_kUp=kUp;
	
	float orientation[]={kHead.x,kHead.y,kHead.z, kUp.x,kUp.y,kUp.z};

	alListenerfv(AL_POSITION, &kPos[0]);
	alListenerfv(AL_ORIENTATION, orientation);
}


int OpenAlSystem::GetUnusedSource()
{
	for(unsigned int i=0;i<m_kSources.size();i++) {
		if(m_kSources[i]->m_bUsed==false){
			return i;
		}
	}

	return -1;
}

void OpenAlSystem::AddSound(Sound* pkSound) 
{
//	cout<<"Adding a sound"<<endl;
	for(list<Sound*>::iterator it=m_akSounds.begin();it!=m_akSounds.end();it++)
	{	
		if( (*it) == pkSound)
		{
//			cout<<"already added"<<endl;
			return;
		}
	}
	
	m_akSounds.push_back(pkSound);
}

void OpenAlSystem::RemoveSound(Sound* pkSound) 
{
//	cout<<"Removing a sound"<<endl;
	if(pkSound==NULL)
		return;
	
	
	bool found=false;
	for(list<Sound*>::iterator it=m_akSounds.begin();it!=m_akSounds.end();it++)
	{	
		if( (*it) == pkSound)
			found=true;	
	}
	
	if(found==true)
	{
	
		if(pkSound->m_iSource!=-1){
			alSourceStop(m_kSources[pkSound->m_iSource]->m_iSource);				
			m_kSources[pkSound->m_iSource]->m_bUsed=false;
			pkSound->m_iSource=-1;
		}
		pkSound->m_iNrOfPlays++;
		m_akSounds.remove(pkSound);
	}
}

void OpenAlSystem::Update()
{

	//Gubb was here
	m_pkMusic->Update();
	
	vector<Sound*> kPlay;
	vector<Sound*> kRemove;
	
	kPlay.clear();
	kRemove.clear();

	for(list<Sound*>::iterator it=m_akSounds.begin();it!=m_akSounds.end();it++) {
		Sound *Current=(*it);
	
		int status=GetState(Current);

		if(status==-1){
//			cout<<"sound starting"<<endl;
			//Sound thats going to be played
			if(Hearable(Current)){
				kPlay.push_back(Current);
				continue;
			}		
		}

		if(status==AL_PLAYING){
//			cout<<"spelar"<<endl;
			if(Hearable(Current)){
//				cout<<"updating sound"<<endl;
				alSourcefv(m_kSources[Current->m_iSource]->m_iSource, AL_POSITION,&Current->m_kPos[0]);	
				alSourcefv(m_kSources[Current->m_iSource]->m_iSource, AL_VELOCITY,&Current->m_kVel[0]);	
		
			//sounds that cant be heard anymore
			} else {
				//if int aint looping remove it
				if(Current->m_bLoop==false){
					kRemove.push_back(Current);					
					continue;
				//else stop it and dealocate the sound source
				} else {
					//cout<<"Sound disapeared in the distance"<<endl;
					alSourceStop( m_kSources[Current->m_iSource]->m_iSource );
					m_kSources[Current->m_iSource]->m_bUsed=false;
					Current->m_iSource=-1;
					continue;				
				}				
			}			
		}
		
		if(status==AL_STOPPED){
//			cout<<"sound stoped"<<endl;
			if(Current->m_bLoop==false){
			
				kRemove.push_back(Current);
				continue;
			}		
		}
	}
	
	unsigned int i;
	for(i=0;i<kRemove.size();i++){
		RemoveSound(kRemove[i]);	
	}
		
	for(i=0;i<kPlay.size();i++){
		
		int iSource=GetUnusedSource();
		
		if(iSource==-1)
			break;
			
		PlaySound(kPlay[i],iSource);
	
	}
	
}


bool OpenAlSystem::Hearable(Sound* pkSound)
{
	if(  (pkSound->m_kPos-m_kPos).Length() < 100 ) 
		return true;
	else
		return false;
}


ALint OpenAlSystem::GetState(Sound* pkSound)
{
	if(pkSound->m_iSource == -1)
		return -1;
	
	ALint state;	
	alGetSourcei(m_kSources[pkSound->m_iSource]->m_iSource,AL_SOURCE_STATE,&state);
	// alGetSourceiv ???
	return state;
}


void OpenAlSystem::PlaySound(Sound* pkSound,int iSource) 
{
	char fil[256];	
	int sound;
	
	m_kSources[iSource]->m_bUsed=true;	
	pkSound->m_iSource=iSource;

	strcpy(fil,pkSound->m_acFile.c_str());
	sound=sbm->LoadSound(fil);
	if(sound==-1) {
		cout<<"somethings went wrong when trying to load this sound"<<endl;
		RemoveSound(pkSound);
		return;	
	};
	
  	alSourcei(m_kSources[iSource]->m_iSource, AL_BUFFER, sound);	
   alSourcef(m_kSources[iSource]->m_iSource, AL_REFERENCE_DISTANCE, 3.5);
	alSourcefv(m_kSources[iSource]->m_iSource, AL_POSITION,&pkSound->m_kPos[0]);	
	alSourcefv(m_kSources[iSource]->m_iSource, AL_VELOCITY,&pkSound->m_kVel[0]);	
   
   if(pkSound->m_bLoop)
	   alSourcei(m_kSources[iSource]->m_iSource, AL_LOOPING, 1);
	else
		alSourcei(m_kSources[iSource]->m_iSource, AL_LOOPING, 0);

	
	alSourcePlay(m_kSources[iSource]->m_iSource);
//	cout<<"Starting to play"<<endl;
}

void OpenAlSystem::RunCommand(int cmdid, const CmdArgument* kCommand)
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
				m_pkMusic->SetVolume((float) strtod( (kCommand->m_kSplitCommand[1]).c_str(),NULL));
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



