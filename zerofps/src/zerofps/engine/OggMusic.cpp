// OggMusic.cpp: implementation of the OggMusic class.
//
//////////////////////////////////////////////////////////////////////

#include "oggmusic.h"
#include "zerofps.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



OggMusic::OggMusic() : 
	ZFObject("OggMusic")  
{
	OggMusic(4,4096);
}


OggMusic::OggMusic(unsigned int uiNrOfBuffers, unsigned int uiBufferSize) : 
	m_bFileOK(false),
	m_bPlaying(false),
	m_uiBufferSize(uiBufferSize),
	m_uiNrOfBuffers(uiNrOfBuffers),
	m_bLooping(true),
	ZFObject("OggMusic")

{
	
	m_pcTempBuffer = new char[uiBufferSize]; 
    m_pALuiBuffers = new ALuint[uiNrOfBuffers];
	alGenBuffers(m_uiNrOfBuffers,m_pALuiBuffers);
	if (alGetError()!=AL_NO_ERROR)
	{
		cout<< "error generating buffers!" <<endl;
	}
	alGenSources(1, &m_ALuiSource);
	
	
	
	Vector3 pos(0,0,0);
	alSourcefv(m_ALuiSource, AL_POSITION,&pos[0]);		
	alSourcefv(m_ALuiSource,  AL_VELOCITY,&pos[0]);	
	
	if (alGetError()!=AL_NO_ERROR)
	{
		cout<<"error generating sources!" <<endl;
	}

//	if((SDL_Init(SDL_INIT_TIMER)==-1))  
//       cout<<"Could not initialize SDL:"<< SDL_GetError();
   m_pkZeroFpsObject	= static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	if(m_pkZeroFpsObject==NULL) 
	{
		cout<<"OggMusic ERROR : could not retrive ZeroFps object" <<endl;
	}
	
    	
}

OggMusic::~OggMusic()
{
	
	//SDL_Quit();
	
	//denna onda funktion krashar när den anropas här
	//Stop();

	alDeleteBuffers(m_uiNrOfBuffers,m_pALuiBuffers);
	alDeleteSources(1, &m_ALuiSource);
	delete m_pcTempBuffer;
	delete []m_pALuiBuffers;
	
	if(m_bFileOK)
		ov_clear(&m_kOggFile);
}


bool OggMusic::LoadFile(string kFileName)
{
	if(m_bPlaying)
		Stop();
	if(m_bFileOK)
		ov_clear(&m_kOggFile);
	
	ZFVFile File;
	//FILE* File;
	//File = fopen(kFileName.c_str() , "rb" );
	if(!File.Open(kFileName,0, false))
	{
		cout<<"error opening file: " <<kFileName <<endl;
		return false;
	} 
	
	else
	{
		if(ov_open(File.m_pkFilePointer,&m_kOggFile ,NULL,0)<0)
		{
			cout<<"File not valid ogg:" <<kFileName <<endl;
			//fclose(File);
			return false;
		}
		else
		{
			//NULLing the filepointer so it doesent get closed when 
			//the fileobject goes out of scope. its the OGGs file now! muahaha!
			File.m_pkFilePointer = NULL;
			//displaying some Ogg file info.. 
			cout<<"------------------------" <<endl  
				<<"File info for: " <<kFileName <<endl;
			long nrstreams = ov_streams(&m_kOggFile);
			cout<<"number of bitstreams:" <<nrstreams <<endl;
			
			vorbis_info* info = ov_info(&m_kOggFile, 0);
			cout<<"sampling rate:"<<info->rate <<endl
				<<"channels:" <<info->channels <<endl
				<<"bitrate:" <<info->bitrate_nominal <<endl; 
			if(ov_seekable(&m_kOggFile) > 0) 
				 cout<<"seekable" <<endl;
			else 
				cout<<"not seekable" <<endl;
				
			cout	<<"------------------------" <<endl;
			//check if i want to use the file..
			if(nrstreams != 1) 
			{
				cout<<"i dont want to play this file. to many bitstreams!" <<endl;
				return false;
			}
			if(info->bitrate_upper != info->bitrate_lower)	
			{
				cout<<"i dont want to play this file. its got an unfixed bitrate!" <<endl;
				return false;
			}
			if(info->channels != 2)	
			{
				cout<<"i dont want to play this file. i dont do MONO!" <<endl;
				return false;
			}
			//saving the sampling rate
			m_uiSamplingRate = info->rate;
			//if you got this far you are lucky 
			cout<<"file seems OK" <<endl;
			m_bFileOK=true;
			return true;
		}
	}
}


bool OggMusic::Play()
{
	
	if(m_bPlaying)
		Stop();
	if(!m_bFileOK)
		return false;
	
	
	for(unsigned int index = 0; index<m_uiNrOfBuffers; index++)
	{
		QueueBuffer(&m_pALuiBuffers[index]);
	}
	m_fStartTime = GetTicks();
	alSourcePlay(m_ALuiSource);	
	
	if (alGetError()!=AL_NO_ERROR)
	{
		cout<<"error playing!" <<endl;
		return false;
	} 
	m_bPlaying=true;
	//ov_pcm_seek(&m_kOggFile,0);
	
	return true;
}

bool OggMusic::Update()
{

	
	if(m_bPlaying)
	{
		int iPlaying=AL_PLAYING;
		alGetSourcei(m_ALuiSource, AL_SOURCE_STATE, &iPlaying);
		if(iPlaying!=AL_PLAYING)
		{

			ov_time_seek(&m_kOggFile, (GetTicks() - m_fStartTime));	
			alSourcePlay(m_ALuiSource);	
		}

		int iBuffersInQueue = m_uiNrOfBuffers;
		int iProcessed = 0;
		ALuint ALuiBufferID;
		alGetSourcei(m_ALuiSource, AL_BUFFERS_PROCESSED, &iProcessed);
		//cout<<"pros: " <<iProcessed;
		if(iProcessed>0)
		{
			while(iProcessed)
			{
				alSourceUnqueueBuffers(m_ALuiSource, 1 , &ALuiBufferID);	
				if(QueueBuffer(&ALuiBufferID))
				{
					iProcessed--;
				}
				else 
				{
					iBuffersInQueue--;
					if(iBuffersInQueue == 0)
					{
						Stop();
						return false;
					}
				}
			}
		return true;
		}
	}
	return false;
}

bool OggMusic::Stop()
{
	int iPlaying;
	alGetSourcei(m_ALuiSource, AL_SOURCE_STATE, &iPlaying);
	if(iPlaying==AL_PLAYING)
	{
		alSourceStop(m_ALuiSource);
		ALint iBuffersQueued;
		ALuint ALuiBufferID;
		alGetSourcei(m_ALuiSource, AL_BUFFERS_QUEUED, &iBuffersQueued);	
		//cout<<"buffers still in queue:" <<iBuffersQueued <<endl;
		while(iBuffersQueued > 0)
		{
			alSourceUnqueueBuffers(m_ALuiSource, 1, &ALuiBufferID);
			iBuffersQueued--;
		}
	}
	m_bPlaying=false;
	//rewind

//	Dvoid   krashar FORTFARANDE här för mig =(
	if(ov_pcm_seek(&m_kOggFile,0) != 0)
		return false;

	return true;
}

bool OggMusic::QueueBuffer(ALuint *pALuiBuffer)
{
	int current_section;
	long bytes_read = ov_read(&m_kOggFile, m_pcTempBuffer, m_uiBufferSize,0,2,1, &current_section); 
	if(!(bytes_read==OV_HOLE || bytes_read==OV_EBADLINK ))
	{
		if(bytes_read == 0)
		{
			if(m_bLooping)
			{
				if (ov_pcm_seek(&m_kOggFile,0) == 0)
					return QueueBuffer(pALuiBuffer);
				else 
					cout<<"faild to loop OGG!" <<endl;
			}
			return false;
		}
		alBufferData(*pALuiBuffer, AL_FORMAT_STEREO16, m_pcTempBuffer, bytes_read, m_uiSamplingRate);  
		if (alGetError()!=AL_NO_ERROR)
		{
			cout<<"error generating bufferDATA!" <<endl;
			return false;
		}
		
		alSourceQueueBuffers(m_ALuiSource, 1,pALuiBuffer );
		if (alGetError()!=AL_NO_ERROR)
		{
			cout<<"error queuing bufferDATA!" <<endl;
			return false;
		}		
		return true;
	}
	else 
		return false;
}

bool OggMusic::SetVolume(float fVolume)
{
	if((fVolume<=1.0) && (fVolume>0.0))
	{
		alSourcef(m_ALuiSource, AL_GAIN, fVolume);
		return true;
	}
	return false;
};

float OggMusic::GetTicks() 
{
	return (m_pkZeroFpsObject->GetGameTime());
};

