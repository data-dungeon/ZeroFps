#include "audiomanager.h"

AudioManager::AudioManager(ZeroFps* pkZeroFps)
: ZFObject("AudioManager"){
	m_pkFile = pkZeroFps->m_pkFile;	
	m_pkZeroFps =pkZeroFps;
	m_pkMusic= new Music(m_pkFile);
	InitAudio();

	g_ZFObjSys.Register_Cmd("music",FID_MUSICSET,this);
}

void AudioManager::LoadMusic(char* acFile) {
	m_pkMusic->Load(acFile);
}

void AudioManager::PlayMusic() {
	if(m_baudio_open)
		m_pkMusic->Start();
}

void AudioManager::StopMusic() {
	if(m_baudio_open)
		m_pkMusic->Stop();
}


void AudioManager::InitAudio(){																//denna funktion initierar ljudet
	m_iaudio_rate=			22050;
	m_iaudio_format=		AUDIO_S16;
	m_iaudio_channels=	2;
	m_iaudio_buffers=		4096;
	m_baudio_open=			false;
	
	
	//Open the audio device 
	m_baudio_open = true;
	if(Mix_OpenAudio(m_iaudio_rate,m_iaudio_format,m_iaudio_channels,m_iaudio_buffers) < 0) {
		m_pkZeroFps->m_pkConsole->Print("Couldn't open audio");
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		m_baudio_open=false;
	} else {
		Mix_QuerySpec(&m_iaudio_rate, &m_iaudio_format, &m_iaudio_channels);
		m_pkZeroFps->m_pkConsole->Print("Open Audio");					
		printf("Opened audio at %d Hz %d bit %s, %d bytes audio buffer\n", m_iaudio_rate,
			(m_iaudio_format&0xFF),
			(m_iaudio_channels > 1) ? "stereo" : "mono",
			m_iaudio_buffers );
	}
	
}
	
void AudioManager::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) {
		case FID_MUSICSET:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				//Print("Syntax: music 1/0");
				return;
				}
			
			if(kCommand->m_kSplitCommand[1]=="1") {
				cout << "Run music CMD: PlayMusic" << endl;
				PlayMusic();
			}
			if(kCommand->m_kSplitCommand[1]=="0")
				StopMusic();
			break;

		}
}
	
	
	
	
	
	

