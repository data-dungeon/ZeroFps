#ifndef _ENGINE_AUDIOSYSTEM_H_
#define _ENGINE_AUDIOSYSTEM_H_ 

#include <list>
#include <map>
#include <string.h>
#include <string>
#include "../engine_systems_x.h"
#include <AL/al.h>
#include <AL/alut.h>
#include "oggmusic.h"
#include "../../basic/vector3.h"
#include "../../basic/vector4.h"
#include "../../basic/zfresource.h"

#define DUMMY_SOUND "/data/sound/dummy.wav"
#define MAX_NUM_SOURCES 10

class ZFSound;

class ENGINE_SYSTEMS_API ZFAudioSystem : public ZFSubSystem 
{		
	public:
		ZFResourceHandle* GetResHandle(string strFileName);
		void AddSound(ZFSound* pkSound);
		void RemoveSound(ZFSound* pkSound);
		void Update();

		ZFAudioSystem();
		~ZFAudioSystem();
		
		void Init();
		void SetListnerPosition(Vector3 kPos,Vector3 kAngel,Vector3 kUp);
		
		bool StartUp();
		bool ShutDown();
		bool IsValid();

	private:
		struct SourceAlloc
		{
			ALuint m_iSource;		
			bool m_bUsed;
		};
		
		Vector3 m_kPos, m_kHead;
		Vector4 m_kUp;
		
		list<ZFSound*> m_akSounds;
		vector<SourceAlloc*> m_kSources;
		
		int GetUnusedSource();
		void GenerateSources(int iNrOfSources);
		bool Hearable(ZFSound* pkSound);
		ALint GetState(ZFSound* pkSound);

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

		//vector<ZFResourceHandle*> m_vkResHandles;
		map<string, ZFResourceHandle*> m_mkResHandles;
};

class ENGINE_SYSTEMS_API ZFSound : public ZFResource
{
	public:
		ZFSound();
		~ZFSound();
		bool Create(string strName);	// overloaded

		int CalculateSize();

		Vector3 m_kPos;					// sounds position
		Vector3 m_kVel;					// sounds velocity
		bool m_bLoop;						// does the sound loop?	
		
	private:

		bool Start(int iSourceIndex, int iSourceName);
		bool Load();

		ALuint m_uiIndex;
		char* m_szFileName;
		int m_iSourceIndex;				// index in m_kSources, if no source it shud be -1
		int m_iNrOfPlays;					// added by 1 each time this sound is played		
		int m_iPriority;	

		friend class ZFAudioSystem;	
};

ENGINE_SYSTEMS_API ZFResource* Create__WavSound();

#endif



