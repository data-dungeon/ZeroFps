#ifndef _AUDIOMANAGER_H_
#define _AUDIOMANAGER_H_

#include "../basic/basic.pkg"
#include "engine.pkg"
#include "music.h"
#include <iostream>
#include <SDL/SDL_mixer.h>		

class Music;

/// Engine Systems that runs music and sound.
class ENGINE_API AudioManager  : public ZFObject{
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
		enum FuncId_e
			{
			FID_MUSICSET,
			};

		void RunCommand(int cmdid, const CmdArgument* kCommand);

		AudioManager(ZeroFps* pkZeroFps);
	
		void LoadMusic(char* acFile);
		void PlayMusic();
		void StopMusic();
		
	
};

#endif





