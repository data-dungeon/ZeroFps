#ifndef _OPENALSYSTEM_H_
#define _OPENALSYSTEM_H_ 

//#include "vector3.h"
#include "../basic/basic.pkg"
#include <vector>
#include <list>
#include <string.h>
#include <string>
#include "soundbuffermanager.h"


class Sound{
	public:
		string m_acFile;			//wav file to play
		Vector3 m_kPos;			//sounds position
		Vector3 m_kVel;			//sounds velocity
		bool m_bLoop;				//does the sound loop?
		int m_iSource;				//index in m_kSources, if no source it shud be -1
		int m_iNrOfPlays;       //added by 1 each time this sound is played		
		int m_iPriority;			

		Sound();
};


class OpenAlSystem{
	private:
		struct SourceAlloc{
			ALuint m_iSource;		
			bool m_bUsed;
		};
		
		Vector3 m_kPos;
		Vector3 m_kAngel;
		
		list<Sound*> m_akSounds;		
		vector<SourceAlloc*> m_kSources;
		
		SoundBufferManager *sbm;
		
		int GetUnusedSource();
		void GenerateSources(int iNrOfSources);
		bool Hearable(Sound* pkSound);
		ALint GetState(Sound* pkSound);
		void PlaySound(Sound* pkSound, int iSource);
		
	public:
		OpenAlSystem();
		~OpenAlSystem();
		
		void Init();
		void SetListnerPosition(Vector3 kPos,Vector3 kAngel);
		
		void AddSound(Sound* pkSound);
		void RemoveSound(Sound* pkSound);
		void Update();
};

#endif



