#ifndef _ENGINE_MUSIC_H
#define _ENGINE_MUSIC_H

/*		
		Denna class sk�ter uppspelingen av musiken.
		vanligtvis s� laddas och startas musiken med hj�lp av en Event som map classen skapar
		Den laddar in musiken av valfrit format som SDL_Mixer har st�d f�r.
		file funktionen �ppnar filen och laddar musiken-
		start startar uppspelningen av musiken.
		och stop stoppar musiken.
		close stoppar musiken och st�nger av ljudkortet.
		
*/
		
#include <SDL/SDL_mixer.h>		
#include <iostream>
#include "../engine_systems_x.h"
#include <iostream>

using namespace std;

class ENGINE_SYSTEMS_API Music{
	private:
		Mix_Music *mus;				//pekare till den laddade musiken
		char *musicfile;
//		bool m_audio_open;	

		
	public:
		Music();
		void Start(void);
		void Load(char* nmusicfile);
		void Stop(void);
//		void Close(void);
		

		
};

#endif




