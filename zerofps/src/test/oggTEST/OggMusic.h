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

using std::cout;
using std::endl;
using std::string;

class OggMusic  
{
public:
	OggMusic(); 
	OggMusic(string kFileName);
	virtual ~OggMusic();
	bool LoadFile(string kFileName);
	bool Play();
private:
	bool InitPlay();

	OggVorbis_File m_kOggFile;
	bool m_bFileOK;
	bool m_bPlaying;
	unsigned int m_uiNrOfBuffers;
	unsigned int m_uiBufferSize;
	////ALuint* m_pALuiSource;
	ALuint m_ALuiSource;
	ALuint* m_pALuiBuffers;
};

#endif // !defined(AFX_OGGMUSIC_H__F6B21359_4F71_4820_B204_C24F0BEE49A7__INCLUDED_)
