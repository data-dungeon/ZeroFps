#include "openalsystem.h"

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
{
	sbm=new SoundBufferManager();

	Init();
	GenerateSources(10);
	
}

OpenAlSystem::~OpenAlSystem()
{
	for(int i=0;i<m_kSources.size();i++) {
		delete m_kSources[i];
	}
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

	SetListnerPosition(Vector3(0,0,0),Vector3(0,0,0));

}


void OpenAlSystem::SetListnerPosition(Vector3 kPos,Vector3 kAngel) 
{
	m_kPos=kPos;
	m_kAngel=kAngel;

	alListenerfv(AL_POSITION, &kPos[0]);
	alListenerfv(AL_ORIENTATION, &kAngel[0]);
}


int OpenAlSystem::GetUnusedSource()
{
	for(int i=0;i<m_kSources.size();i++) {
		if(m_kSources[i]->m_bUsed==false){
			return i;
		}
	}
	return -1;
}

void OpenAlSystem::AddSound(Sound* pkSound) 
{
	cout<<"Adding a sound"<<endl;
	m_akSounds.push_back(pkSound);
}

void OpenAlSystem::RemoveSound(Sound* pkSound) 
{
	cout<<"Removing a sound"<<endl;
	
	alSourceStop(m_kSources[pkSound->m_iSource]->m_iSource);
	if(pkSound->m_iSource!=-1){
		m_kSources[pkSound->m_iSource]->m_bUsed=false;
		pkSound->m_iSource=-1;
	}
	pkSound->m_iNrOfPlays++;
	m_akSounds.remove(pkSound);
}

void OpenAlSystem::Update()
{

	vector<Sound*> kPlay;
	vector<Sound*> kRemove;
	
	kPlay.clear();
	kRemove.clear();

	for(list<Sound*>::iterator it=m_akSounds.begin();it!=m_akSounds.end();it++) {
		Sound *Current=(*it);
	
		int status=GetState(Current);

		if(status==-1){
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
					cout<<"Sound disapeared in the distance"<<endl;
					alSourceStop( m_kSources[Current->m_iSource]->m_iSource );
					m_kSources[Current->m_iSource]->m_bUsed=false;
					Current->m_iSource=-1;
					continue;				
				}				
			}			
		}
		
		if(status==AL_STOPPED){
			if(Current->m_bLoop==false){
			
				kRemove.push_back(Current);
				continue;
			}		
		}
	}
	
	for(int i=0;i<kRemove.size();i++){
		RemoveSound(kRemove[i]);	
	}
		
	for(int i=0;i<kPlay.size();i++){
		
		int iSource=GetUnusedSource();
		
		if(iSource==-1)
			break;
			
		PlaySound(kPlay[i],iSource);
	
	}
	
}


bool OpenAlSystem::Hearable(Sound* pkSound)
{
	if(  (pkSound->m_kPos-m_kPos).Length() < 50 ) 
		return true;
	else
		return false;
}


ALint OpenAlSystem::GetState(Sound* pkSound)
{
	if(pkSound->m_iSource == -1)
		return -1;
	
	ALint state;	
	alGetSourceiv(m_kSources[pkSound->m_iSource]->m_iSource,AL_SOURCE_STATE,&state);
	
	return state;
}


void OpenAlSystem::PlaySound(Sound* pkSound,int iSource) 
{
	char fil[256];	
	
	m_kSources[iSource]->m_bUsed=true;	
	pkSound->m_iSource=iSource;

	strcpy(fil,pkSound->m_acFile.c_str());
	
  	alSourcei(m_kSources[iSource]->m_iSource, AL_BUFFER, sbm->LoadSound(fil));	
   alSourcef(m_kSources[iSource]->m_iSource, AL_REFERENCE_DISTANCE, 10);
	alSourcefv(m_kSources[iSource]->m_iSource, AL_POSITION,&pkSound->m_kPos[0]);	
	alSourcefv(m_kSources[iSource]->m_iSource, AL_VELOCITY,&pkSound->m_kVel[0]);	
   
   if(pkSound->m_bLoop)
	   alSourcei(m_kSources[iSource]->m_iSource, AL_LOOPING, 1);
	else
		alSourcei(m_kSources[iSource]->m_iSource, AL_LOOPING, 0);

	
	alSourcePlay(m_kSources[iSource]->m_iSource);
}



