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
   
	ALsizei size, freq, bits;
   ALenum format;
   ALvoid *data;
   ALboolean err;

   err = alutLoadWAV(io->File(acFile), &data, &format, &size, &bits, &freq);
   if(err == AL_FALSE) {
//	   fprintf(stderr, "Could not load %s\n", filename);
	   cout<<"cold not load file"<<endl;
	   delete temp;
	   return -1;
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
		int j=Load(acFile);
		if(j==-1){
			cout<<"Fuck!! Ass!!! something went wrong,when i tryed to load a sound"<<endl;			
			exit(1);
		};
		return j;		
	}
}


int SoundBufferManager::GetIndex(char* acFile) 
{
	for(int i=0;i<m_kBuffers.size();i++) {
		if(m_kBuffers[i]->m_acFile==acFile)
			return m_kBuffers[i]->m_iIndex;	
	}	
	
	return -1;
}


void SoundBufferManager::ClearAll() 
{
	for(int i=0;i<m_kBuffers.size();i++) {
		alDeleteBuffers(1,&m_kBuffers[i]->m_iIndex);
		delete m_kBuffers[i];
	}	
	
	m_kBuffers.clear();
}


