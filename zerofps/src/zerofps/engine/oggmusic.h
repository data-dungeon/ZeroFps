// OggMusic.h: interface for the OggMusic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGGMUSIC_H__F6B21359_4F71_4820_B204_C24F0BEE49A7__INCLUDED_)
#define AFX_OGGMUSIC_H__F6B21359_4F71_4820_B204_C24F0BEE49A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <al/alut.h>
#include <al/alc.h>
#include <vorbis/vorbisfile.h>
#include <string>
#include <iostream>
#include <conio.h>
//#include <sdl.h>
#include "../basic/basic.pkg"
#include "engine_x.h"
//#include "zerofps.h"

/*using std::cout;
using std::endl;
using std::string;**/
using namespace std;

class ZeroFps;
class OggMusic: public ZFObject  
{
public:
	
	bool Stop();
	bool Update();
	OggMusic(); 
	OggMusic(unsigned int uiNrOfBuffers, unsigned int uiBufferSize); 
	virtual ~OggMusic();
	bool LoadFile(string kFileName);
	bool Play();
	bool SetVolume(float fVolume);
	inline void SetLooping(bool bState) {m_bLooping=bState;};

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
};

#endif // !defined(AFX_OGGMUSIC_H__F6B21359_4F71_4820_B204_C24F0BEE49A7__INCLUDED_)
