// OggMusic.cpp: implementation of the OggMusic class.
//
//////////////////////////////////////////////////////////////////////

#include "OggMusic.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OggMusic::OggMusic()  
{
	OggMusic(4,4096);
}


OggMusic::OggMusic(unsigned int uiNrOfBuffers, unsigned int uiBufferSize) : 
	m_bFileOK(false),
	m_bPlaying(false),
	m_uiBufferSize(uiBufferSize),
	m_uiNrOfBuffers(uiNrOfBuffers),
	m_bLooping(true)
{
	
	m_pcTempBuffer = new char[uiBufferSize]; 
    m_pALuiBuffers = new ALuint[uiNrOfBuffers];
	alGenBuffers(m_uiNrOfBuffers,m_pALuiBuffers);
	if (alGetError()!=AL_NO_ERROR)
	{
		cout<<"error generating buffers!" <<endl;
	}
	alGenSources(1, &m_ALuiSource);
	if (alGetError()!=AL_NO_ERROR)
	{
		cout<<"error generating sources!" <<endl;
	}
}

OggMusic::~OggMusic()
{
	//	alDeleteBuffers(m_uiNrOfBuffers,m_pALuiBuffers);
	//	alDeleteSources(1, &m_ALuiSource);
	delete [] m_pcTempBuffer;
	delete m_pALuiBuffers;
	if(m_bFileOK)
		ov_clear(&m_kOggFile);
}


bool OggMusic::LoadFile(string kFileName)
{
	if(m_bPlaying)
		Stop();
	if(m_bFileOK)
		ov_clear(&m_kOggFile);
	
	FILE* File;
	File = fopen(kFileName.c_str() , "rb" );
	if(File==NULL)
	{
		cout<<"error opening file: " <<kFileName <<endl;
		return false;
	} 
	
	else
	{
		if(ov_open(File,&m_kOggFile ,NULL,0)<0)
		{
			cout<<"File not valid ogg:" <<kFileName <<endl;
			fclose(File);
			return false;
		}
		else
		{
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

/*
bool OggMusic::InitPlay()
{
	char *TempBuffer = new char[44096];
	int current_section = 1;
	long bytes_read = ov_read(&m_kOggFile, TempBuffer, 44096,0,2,1, &current_section); 
	cout<<bytes_read;
	if(!(bytes_read == 0|| bytes_read==OV_HOLE || bytes_read==OV_EBADLINK ))
	{
		alBufferData(*m_pALuiBuffers, AL_FORMAT_STEREO16, &TempBuffer, bytes_read, m_uiSamplingRate);  
		if (alGetError()!=AL_NO_ERROR)
		{
			cout<<"error generating bufferDATA!" <<endl;
		}
		
		m_uiNrOfBuffers = 1;
		alSourceQueueBuffers(m_ALuiSource, 1,m_pALuiBuffers );
		if (alGetError()!=AL_NO_ERROR)
		{
			cout<<"error queuing bufferDATA!" <<endl;
		}		
		alSourcePlay(m_ALuiSource);	
		if (alGetError()!=AL_NO_ERROR)
		{
			cout<<"error playing!" <<endl;
		}		
		//alSourceQueueBuffers(m_ALuiSource, *m_pALuiBuffers, &m_uiNrOfBuffers);
		return true;
	}
	return false;
}*/

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
	alSourcePlay(m_ALuiSource);	
	if (alGetError()!=AL_NO_ERROR)
	{
		cout<<"error playing!" <<endl;
	}		
	
	m_bPlaying=true;
	return true;
}

bool OggMusic::Update()
{
	if(m_bPlaying)
	{
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
						m_bPlaying=false;
						return false;
					}
				}
			}
		}
	}
	return true;
}

bool OggMusic::Stop()
{
	alSourceStop(m_ALuiSource);
	//rewind
	if (ov_pcm_seek(&m_kOggFile,0) != 0)
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
		}
		
		alSourceQueueBuffers(m_ALuiSource, 1,pALuiBuffer );
		if (alGetError()!=AL_NO_ERROR)
		{
			cout<<"error queuing bufferDATA!" <<endl;
		}		
		return true;
	}
	else 
		return false;
}

bool OggMusic::SetVolume(float fVolume)
{
	if(fVolume<=1.0)
	{
		alSourcef(m_ALuiSource, AL_GAIN, fVolume);
		return true;
	}
	return false;
};