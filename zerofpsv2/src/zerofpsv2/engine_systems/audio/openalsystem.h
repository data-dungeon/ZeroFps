#ifndef _ENGINE_OPENALSYSTEM_H_
#define _ENGINE_OPENALSYSTEM_H_ 

//#include "vector3.h"
#include <vector>
#include <list>
#include <string.h>
#include <string>
#include "soundbuffermanager.h"
#include "../engine_systems_x.h"
#include <AL/al.h>
#include <AL/alut.h>
#include "oggmusic.h"

#include "../../basic/vector3.h"
#include "../../basic/vector4.h"

class ENGINE_SYSTEMS_API Sound {
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


class ENGINE_SYSTEMS_API OpenAlSystem : public ZFSubSystem {
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
		//Gubb was here//////
		void RunCommand(int cmdid, const CmdArgument* kCommand);
		OggMusic* m_pkMusic;
		enum FuncId_e
		{
			FID_MUSICLOAD,
			FID_MUSICPLAY,
			FID_MUSICVOLUME,
			FID_MUSICBUFFERS,
			FID_MUSICSTOP
		};
		
		////////////////////
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



