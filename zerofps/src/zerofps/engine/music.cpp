#include "music.h"

Music::Music(FileIo* pkFile) {
//m_audio_open=audio_open;
	m_pkFile=pkFile;
	

};

void Music::Start() {				//detta startar spela den valda musiken
	cout <<"starting to play "<<musicfile<<endl;
	Mix_FadeInMusic(mus,-1,2000);
};
void Music::Stop() {					//stoppar musiken
	cout <<"stopping music\n";
	Mix_FadeOutMusic(1500);
};

void Music::Load(char* nmusicfile) {	//detta laddar l�ten i filen nmusicfile
	musicfile=nmusicfile;
	cout <<"change music file to "<<musicfile<<endl;
	/* Load the requested music file */
	mus = Mix_LoadMUS(m_pkFile->File(musicfile));
	if ( mus == NULL ) {
		cout <<"could not load file "<<musicfile<<endl;
//		exit(2);
	}                            
};


/*
void Music::Close() {
	if( Mix_PlayingMusic() ) {
		Mix_FadeOutMusic(1500);		//g�r en fin utfadening, dette m�rks doc inte n�r man st�nger av spelet f�r ljudet st�ngs av innan det har fadat ut
//		SDL_Delay(1500);				//om detta okommenteras s� h�rs utfadeningen
	}
	if (mus) {
		Mix_FreeMusic(mus);
		mus = NULL;
	}
	if (m_audio_open ) {
		Mix_CloseAudio();
		m_audio_open = 0;
	}
}*/







