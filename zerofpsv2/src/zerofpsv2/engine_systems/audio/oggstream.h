#ifndef __oggstream_h__
#define __oggstream_h__

#include <AL/alc.h>
#include <vorbis/vorbisfile.h>
#include "../../basic/vector3.h"
#include <SDL/SDL_thread.h>

class ZSSZeroFps;
class Vector3;
class ZSSAudioSystem;

class OggStream
{

public:
	bool		m_bKillMe;

	bool		IsPlaying()	{ return m_bPlaying; }
	bool		SetVolume(float fVolume);
	float	GetVolume();
	void		SetLooping(bool bState) {m_bLooping=bState;};
	int		GetID() { return m_iID; }
	Vector3	GetPos() { return m_kPos; }

protected:

	OggStream(
		bool bListenerRelCoords = true, 
		unsigned int uiNrOfBuffers = 24, 
		unsigned int uiBufferSize = 4096); 

	~OggStream();

	static int ThreadMain(void *);

	bool	Stop();
	bool	Update();
	bool	Open(string kFileName);
	bool	Play();

	SDL_Thread* m_pkThread;
	string		m_strFileName;
	Vector3		m_kPos;
	int			m_iID;
	bool			m_bListenerRelCoords; // true = spelas alltid där lyssnaren befinner sig
	float			m_fVolume;

	friend class ZSSAudioSystem;

private:

	bool  QueueBuffer(ALuint *pALuiBuffer);
	bool  Create();

	OggVorbis_File		m_kOggFile;
	ALenum				m_eFormat;
	vorbis_info*		m_pkVorbisInfo;
	vorbis_comment*	m_pkVorbisComment;
	bool					m_bFileOK;
	bool					m_bPlaying;
	bool					m_bLooping;
	unsigned int		m_uiNrOfBuffers;
	unsigned int		m_uiBufferSize;
	float					m_fStartTime;
	char*					m_pcTempBuffer;
	ALuint				m_uiSource;
	ALuint*				m_pALuiBuffers;
	ZSSZeroFps*			m_pkZeroFpsObject;
	ZSSAudioSystem*	m_pkAudioSys;
};

#endif // __oggstream_h__
