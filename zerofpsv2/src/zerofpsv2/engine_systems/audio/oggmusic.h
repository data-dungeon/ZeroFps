// OggMusic.h: interface for the OggMusic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGGMUSIC_H__F6B21359_4F71_4820_B204_C24F0BEE49A7__INCLUDED_)
#define AFX_OGGMUSIC_H__F6B21359_4F71_4820_B204_C24F0BEE49A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <AL/alut.h>
#include <AL/alc.h>
#include <vorbis/vorbisfile.h>
#include <string>
#include <iostream>
//#include <conio.h>
//#include <sdl.h>
#include "../engine_systems_x.h"
//#include "zerofps.h"
#include "../../basic/zfsystem.h"
#include "../../basic/vector3.h"

#include <SDL/SDL_thread.h>

/*using std::cout;
using std::endl;
using std::string;**/
using namespace std;

class ZeroFps;
class OggMusic;

struct THREAD_INFO
{
	int iType;
	OggMusic* pkOgg;
	void* data;
};

/// SubSystem that handles music in ZeroFPS.
class ENGINE_SYSTEMS_API OggMusic: public ZFSubSystem  
{
public:

	bool Stop();
	bool Update(Vector3 kListerPos);
	bool IsPlaying()	{ return m_bPlaying; }
	OggMusic(); 
	OggMusic(unsigned int uiNrOfBuffers, unsigned int uiBufferSize); 
	virtual ~OggMusic();
	bool LoadFile(string kFileName);
	bool Play();
	bool SetVolume(float fVolume);
	float GetVolume();
	inline void SetLooping(bool bState) {m_bLooping=bState;};

	static int OggMusic::ThreadMain(void *);
	static SDL_Thread* m_pkThread;
	
private:
	//bool InitPlay();
	float GetTicks(); 
	bool QueueBuffer(ALuint *pALuiBuffer);
	OggVorbis_File m_kOggFile;
	bool m_bFileOK;
	bool m_bPlaying;
	bool m_bLooping;
	unsigned int m_uiNrOfBuffers;
	unsigned int m_uiBufferSize;
	unsigned int m_uiSamplingRate;
	float m_fStartTime;
	char *m_pcTempBuffer;
	////ALuint* m_pALuiSource;
	ALuint m_ALuiSource;
	ALuint* m_pALuiBuffers;
	// need those ticks..
	ZeroFps* m_pkZeroFpsObject;


	bool StartUp();
	bool ShutDown();
	bool IsValid();

};

#endif // !defined(AFX_OGGMUSIC_H__F6B21359_4F71_4820_B204_C24F0BEE49A7__INCLUDED_)
