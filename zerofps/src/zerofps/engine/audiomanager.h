#ifndef _AUDIOMANAGER_H_
#define _AUDIOMANAGER_H_

#include <iostream>
#include <SDL/SDL_mixer.h>		
#include "../basic/basic.pkg"
#include "engine.pkg"
#include "music.h"

class Music;

class ENGINE_API AudioManager {
	private:
		Music		*m_pkMusic;
		FileIo	*m_pkFile;
		ZeroFps *m_pkZeroFps;
	
		bool	 	m_baudio_open;  //is the audio open?
		int 		m_iaudio_rate;
		Uint16	m_iaudio_format;
		int 		m_iaudio_channels;
		int 		m_iaudio_buffers;		
		
		void InitAudio();
		
	public:
		AudioManager(ZeroFps* pkZeroFps);
	
		void LoadMusic(char* acFile);
		void PlayMusic();
		void StopMusic();
		
	
};

#endif





