// OggMusic.cpp: implementation of the OggMusic class.
//
//////////////////////////////////////////////////////////////////////

#include "OggMusic.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OggMusic::OggMusic() :	m_bFileOK(false), 
								m_bPlaying(false), 
								m_uiBufferSize(4096), 
								m_uiNrOfBuffers(4) 
								//m_pALuiSource(NULL)
{
	m_pALuiBuffers = new ALuint[m_uiNrOfBuffers];
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

OggMusic::OggMusic(string kFileName)
{
	OggMusic();
	LoadFile(kFileName);
}

OggMusic::~OggMusic()
{
	//alDeleteBuffers(m_uiNrOfBuffers,m_pALuiBuffers);
	//alDeleteSources(1, &m_ALuiSource);
	delete [] m_pALuiBuffers;
	if(m_bFileOK)
		ov_clear(&m_kOggFile);
}



bool OggMusic::LoadFile(string kFileName)
{
	if(m_bPlaying)
		return false;
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
				 <<"bitrate:" <<info->bitrate_nominal <<endl 
			//// << "upper:" <<info->bitrate_upper
			///  << "lower:" <<info->bitrate_lower	<<endl;
				 <<"------------------------" <<endl;
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
				cout<<"i dont want to play this file. i dont dog MONO!" <<endl;
				return false;
			}
			//if you got this far you are lucky 
			cout<<"file seems OK" <<endl;
			m_bFileOK=true;
			return true;
		}
	}
}

bool OggMusic::InitPlay()
{
	
	alSourceQueueBuffers(m_ALuiSource, *m_pALuiBuffers, &m_uiNrOfBuffers);
	return true;
}

bool OggMusic::Play()
{
return true;
}