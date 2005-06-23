#include "oggstream.h"
#include <AL/al.h>
#include "../../basic/zfsystem.h"
#include "../../engine/zerofps.h"
#include "zfaudiosystem.h"

OggStream::OggStream(bool bListenerRelCoords, 
							unsigned int uiNrOfBuffers, 
							unsigned int uiBufferSize) : 
	m_bFileOK(false),
	m_bPlaying(false),
	m_uiBufferSize(uiBufferSize),
	m_uiNrOfBuffers(uiNrOfBuffers),
	m_bLooping(true),
	m_bListenerRelCoords(bListenerRelCoords)
{
	m_bKillMe = false;
	m_pkThread = NULL;
	Create();
}

OggStream::~OggStream()
{
	alDeleteBuffers(m_uiNrOfBuffers,m_pALuiBuffers);
	alDeleteSources(1, &m_uiSource);
	delete m_pcTempBuffer;
	delete[] m_pALuiBuffers;
	
	if(m_bFileOK)
		ov_clear(&m_kOggFile);
}

bool OggStream::Create()	
{ 
   m_pkZeroFpsObject	= static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	if(m_pkZeroFpsObject==NULL) 
	{
		cout<<"OggStream - could not retrive ZeroFps object" <<endl;
		return false;
	}

	m_pkAudioSys = static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));
	if(m_pkAudioSys==NULL) 
	{
		cout<<"OggStream - could not retrive m_pkAudioSys object" <<endl;
		return false;
	}

	m_fVolume = 1.0f;

	

	m_pcTempBuffer = new char[ m_uiBufferSize ]; 
   m_pALuiBuffers = new ALuint[ m_uiNrOfBuffers ];
   
   alGetError();
	alGenBuffers(m_uiNrOfBuffers,m_pALuiBuffers);
	if (alGetError()!=AL_NO_ERROR)
	{
		cout<< "OggStream - error generating buffers!" <<endl;
		return false;
	}
	m_uiSource = m_pkAudioSys->GetNewSource();
	if( m_uiSource == -1)
	{
		cout<< "OggStream - error generating source!" <<endl;
		return false;
	}
	
	if(m_bListenerRelCoords)
	{
		alGetError();
		Vector3 pos(0,0,0), dir(0,1,0);
		alSourcefv(m_uiSource, AL_POSITION,&pos[0]);		
		alSourcefv(m_uiSource,  AL_VELOCITY,&dir[0]);	

		if (alGetError()!=AL_NO_ERROR)
		{
			cout<<"OggStream - error settingup sources!" <<endl;
			return false;
		}
	}

	return true; 
}

bool OggStream::Open(string strFileName)
{
	m_strFileName = strFileName;

	if(m_bPlaying)
		Stop();

	if(m_bFileOK)
		ov_clear(&m_kOggFile);
	
	ZFVFile kFile;
	if(!kFile.Open(strFileName,0, false))
	{
		cout<<"OggStream - Error opening file: " <<strFileName <<endl;
		return false;
	} 
	else
	{
		if(ov_open(kFile.m_pkFilePointer,&m_kOggFile ,NULL,0)<0)
		{
			printf("OggStream - kFile not valid ogg: %s\n", strFileName.c_str());
			return false;
		}
		else
		{
			kFile.m_pkFilePointer = NULL; // NULL the filepointer so it doesent get
													// closed when the fileobject goes out of scope.
			
			m_pkVorbisInfo = ov_info(&m_kOggFile, -1);
			m_pkVorbisComment = ov_comment(&m_kOggFile, -1);

			if(m_pkVorbisInfo->channels == 1)
				m_eFormat = AL_FORMAT_MONO16;
			else
			{
				m_eFormat = AL_FORMAT_STEREO16;
				if(m_bListenerRelCoords == false)
					printf("OggStream - Can't play 3dOgg with a stereo file!\n");
			}
				
			if(ov_streams(&m_kOggFile) != 1) 
			{
				printf("OggStream - Failed to open, to many bitstreams\n");
				return false;
			}

			if(m_pkVorbisInfo->bitrate_upper != m_pkVorbisInfo->bitrate_lower)	
			{
				printf("OggStream - Failed to open, unfixed bitrate\n");
				return false;
			}

			m_bFileOK=true;

			return true;
		}
	}

	return false;
}


bool OggStream::Play()
{	
	if(m_bPlaying)
		Stop();

	if(!m_bFileOK)
		return false;
	
	
	
	for(unsigned int index = 0; index<m_uiNrOfBuffers; index++)
		QueueBuffer(&m_pALuiBuffers[index]);

	alGetError();

	m_fStartTime = m_pkZeroFpsObject->m_pkEntityManager->GetSimTime();
	alSourcef(m_uiSource, AL_GAIN, m_fVolume);
	alSourcePlay(m_uiSource);	
	
	if (alGetError()!=AL_NO_ERROR)
	{
		printf("OggStream - Error playing!\n");
		return false;
	} 
	
	m_bPlaying=true;
	
	return true;
}

int OggStream::ThreadMain(void *v)
{
	if(v)
	{
		OggStream* pkOgg = (OggStream*) v;
		while(!pkOgg->m_bKillMe)
		{
			pkOgg->Update();
			SDL_Delay(100);

		}
	}

	return 0;
}

bool OggStream::Update()
{
	if(m_bPlaying)
	{
		if(m_bListenerRelCoords)
		{
			alSource3f(m_uiSource, AL_POSITION,        0.0, 0.0, 0.0);
			alSource3f(m_uiSource, AL_VELOCITY,        0.0, 0.0, 0.0);
			alSource3f(m_uiSource, AL_DIRECTION,       0.0, 0.0, 0.0);
			alSourcef (m_uiSource, AL_ROLLOFF_FACTOR,  0.0          );
			alSourcei (m_uiSource, AL_SOURCE_RELATIVE, AL_TRUE      );
		}
		else
		{
			alSourcefv(m_uiSource, AL_POSITION,			 &m_kPos[0]);				
			alSource3f(m_uiSource, AL_VELOCITY,        0.0, 0.0, 0.0);
			alSource3f(m_uiSource, AL_DIRECTION,       0.0, 0.0, 0.0);
			alSourcei (m_uiSource, AL_SOURCE_RELATIVE, AL_FALSE);
		}

		float vol = m_pkAudioSys->GetMainVolume()* m_fVolume;
		alSourcef(m_uiSource, AL_GAIN, vol);

		int iPlaying;
		alGetSourcei(m_uiSource, AL_SOURCE_STATE, &iPlaying);
		if(iPlaying!=AL_PLAYING)
		{
			ov_time_seek(&m_kOggFile, 
				(m_pkZeroFpsObject->m_pkEntityManager->GetSimTime() - m_fStartTime));	
			alSourcePlay(m_uiSource);	
		}

		int iBuffersInQueue = m_uiNrOfBuffers, iProcessed = 0;
		ALuint ALuiBufferID;
		alGetSourcei(m_uiSource, AL_BUFFERS_PROCESSED, &iProcessed);

		if(iProcessed>0)
		{
			while(iProcessed)
			{
				alSourceUnqueueBuffers(m_uiSource, 1, &ALuiBufferID);	
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

bool OggStream::Stop()
{
	if(!m_bPlaying)
		return true;

	int iPlaying;
	alGetSourcei(m_uiSource, AL_SOURCE_STATE, &iPlaying);
	if(iPlaying==AL_PLAYING)
	{
		alSourceStop(m_uiSource);
		ALint iBuffersQueued;
		ALuint ALuiBufferID;
		alGetSourcei(m_uiSource, AL_BUFFERS_QUEUED, &iBuffersQueued);	
		while(iBuffersQueued > 0)
		{
			alSourceUnqueueBuffers(m_uiSource, 1, &ALuiBufferID);
			iBuffersQueued--;
		}
	}

	m_bPlaying = false;

	if(ov_raw_seek(&m_kOggFile, 0) != 0) 
		return false;

	return true;
}

bool OggStream::QueueBuffer(ALuint *pALuiBuffer)
{
	int current_section;	
	int iPos = 0;
	
	//loop an dfill buffer
	while(iPos < m_uiBufferSize)
	{
		int iLeft = m_uiBufferSize - iPos;
		char* pkPos = (char*)m_pcTempBuffer;
	
		//read buffer
		long bytes_read = ov_read(&m_kOggFile, &pkPos[iPos], iLeft,0, 2, 1, &current_section); 
		
		//any errors?
		if((bytes_read==OV_HOLE || bytes_read==OV_EBADLINK ))
			return false;
		
		//if end of stream reset stream position if looping
		if(bytes_read == 0)
		{
			if(m_bLooping)
			{
				ov_raw_seek(&m_kOggFile,0);
			}
			else
				return false;
		}
		
		//move buffer position
		iPos += bytes_read;
	}
	

	alGetError();
	alBufferData(*pALuiBuffer, m_eFormat, m_pcTempBuffer, iPos, m_pkVorbisInfo->rate);  
	if(alGetError()!=AL_NO_ERROR)
	{
		//cout<<"bytes "<<bytes_read<<" buffer "<<*pALuiBuffer<< endl;
		cout<<"error generating bufferDATA!" <<endl;
		return false;
	}

	alSourceQueueBuffers(m_uiSource, 1, pALuiBuffer );
	if(alGetError()!=AL_NO_ERROR)
	{
		printf("OggStream - Error Queuing buffer\n");
		return false;
	}		
	
	return true;
}

bool OggStream::SetVolume(float fVolume)
{
	if((fVolume<=1.0) && (fVolume>=0.0))
	{
		m_fVolume = fVolume;
		return true;
	}
	return false;
};

float OggStream::GetVolume()
{
	return m_fVolume;
};