#ifndef _OPENALSYSTEM_H_
#define _OPENALSYSTEM_H_ 

//#include "vector3.h"
#include "../basic/basic.pkg"
#include <vector>
#include <list>
#include <string.h>
#include <string>
#include "soundbuffermanager.h"
#include "engine_x.h"

class ENGINE_API Sound {
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


class ENGINE_API OpenAlSystem : public ZFObject {
	private:
		struct SourceAlloc{
			ALuint m_iSource;		
			bool m_bUsed;
		};
		
		Vector3 m_kPos;
		Vector3 m_kHead;
		Vector4 m_kUp;
		
		list<Sound*> m_akSounds;		
		vector<SourceAlloc*> m_kSources;
		
		SoundBufferManager *sbm;
		
		int GetUnusedSource();
		void GenerateSources(int iNrOfSources);
		bool Hearable(Sound* pkSound);
		ALint GetState(Sound* pkSound);
		void PlaySound(Sound* pkSound, int iSource);
		
	public:
		void AddSound(Sound* pkSound);
		void RemoveSound(Sound* pkSound);
		void Update();

		OpenAlSystem();
		~OpenAlSystem();
		
		void Init();
		void SetListnerPosition(Vector3 kPos,Vector3 kAngel,Vector3 kUp);
		
};

#endif



