#include "soundbuffermanager.h"


SoundBufferManager::SoundBufferManager(FileIo *pkIo)
: ZFObject("SoundBufferManager"){
	io=pkIo;
}


int SoundBufferManager::Load(char* acFile) 
{
	
	SoundBuffer *temp=new SoundBuffer();
	temp->m_acFile=acFile;
	
   // create buffer
   alGetError(); /* clear */
   alGenBuffers(1, &temp->m_iIndex);
   if(alGetError() != AL_NO_ERROR) {
   	delete temp;
	   return -1;
   }
   
	ALsizei size=0;
	ALsizei freq;
//	ALsizei bits;
   ALenum format;
   ALvoid *data;
//   ALboolean err;
	ALboolean loop;   //we dont use this
	
//   err = alutLoadWAV(io->File(acFile), &data, &format, &size, &bits, &freq);

   alutLoadWAVFile((ALbyte*)io->File(acFile),&format,&data, &size, &freq,&loop);
   
//   if(err == AL_FALSE) {
	if(size==0){
	   cout<<"cold not load file "<<acFile<<", Loading Dummy"<<endl;		
		char DummyFile[]="file:../data/sound/dummy.wav";
	   alutLoadWAVFile((ALbyte*)io->File(DummyFile),&format,&data, &size, &freq,&loop);
   }

   alBufferData(temp->m_iIndex, format, data, size, freq);
   
   cout<<"Loaded "<< acFile<<endl;
   
   m_kBuffers.push_back(temp);
   return int(temp->m_iIndex);
}


int SoundBufferManager::LoadSound(char* acFile)
{
	
	int i = GetIndex(acFile);
	
	if( i != -1 ){
		return i;
	} else {			
		return Load(acFile);;		
	}
}


int SoundBufferManager::GetIndex(char* acFile) 
{
	for(unsigned int i=0;i<m_kBuffers.size();i++) {
		if(m_kBuffers[i]->m_acFile==acFile)
			return m_kBuffers[i]->m_iIndex;	
	}	
	
	return -1;
}


void SoundBufferManager::ClearAll() 
{
	for(unsigned int i=0;i<m_kBuffers.size();i++) {
		alDeleteBuffers(1,&m_kBuffers[i]->m_iIndex);
		delete m_kBuffers[i];
	}	
	
	m_kBuffers.clear();
}


