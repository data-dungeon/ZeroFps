// zfaudiosystem.h

#ifndef _THE_ZFAUDIOSYSTEM_H_
#define _THE_ZFAUDIOSYSTEM_H_

#include <map>
#include <list>
#include "../engine_systems_x.h"
#include "../../basic/zfresource.h"
#include "../../basic/zfobject.h"
#include "../../basic/vector3.h"
#include "../../basic/vector4.h"
#include "oggmusic.h"

#define DUMMY_SOUND "/data/sound/dummy.wav"
#define SOURCE_POOL_SIZE 32

class ZFAudioSystem;

/// A Sound in the Resource SubSystem.
class ENGINE_SYSTEMS_API ZFSoundRes : public ZFResource
{
	public:
		ALuint GetBufferIndexName();
		ZFSoundRes();
		~ZFSoundRes();
		bool Create(string strName);	// overloaded
		int CalculateSize();
		
	private:

		bool Load();
		char* m_szFileName;
		ALuint m_uiBufferIndexName;
};

ENGINE_SYSTEMS_API ZFResource* Create__WavSound();

class ENGINE_SYSTEMS_API SoundInfo
{
	public:
		SoundInfo();
		SoundInfo(const char* c_szFile, Vector3 pos, Vector3 dir, bool bLoop=false);
		~SoundInfo();

		bool m_bLoop;
		Vector3 m_kPos;
		Vector3 m_kDir;
		char m_acFile[128]; // ändra inte till string, då uppstår en kopiering bugg!!
			
	private:
		ZFSoundRes* m_pkResource;
		ALuint m_uiSourceBufferName;
		bool m_bLoopingNoLongerHearable;

		friend class ZFAudioSystem;
};

/// Sub System for Sound.
class ENGINE_SYSTEMS_API ZFAudioSystem  : public ZFSubSystem 
{
public:
	void RemoveAllSounds();
	
	bool StartSound(SoundInfo kSound);
	bool RemoveSound(SoundInfo kSound, float fMaxSearchRange = 1000000.0f);

	unsigned int GetNumActiveSounds();
	unsigned int GetNumActiveChannels();

	ZFAudioSystem();
	virtual ~ZFAudioSystem();

	//
	// Pure virtual functions derived from ZFSubSystem
	//
	bool StartUp();
	bool ShutDown();
	bool IsValid();

	//
	// Ogg music.
	//
	enum FuncId_e
	{
		FID_MUSICLOAD,
		FID_MUSICPLAY,
		FID_MUSICVOLUME,
		FID_MUSICBUFFERS,
		FID_MUSICSTOP
	};

	void RunCommand(int cmdid, const CmdArgument* kCommand);

	//
	// Common
	//
	void Update();
	void SetListnerPosition(Vector3 kPos,Vector3 kHead,Vector3 kUp);
	Vector3 GetListnerPos() { return m_kPos;  }
	Vector3 GetListnerDir() { return m_kHead; }

private:
	bool m_bIsValid;
	
	OggMusic* m_pkMusic;

	Vector3 m_kPos, m_kHead;
	Vector4 m_kUp;
	
	typedef pair<ALuint, bool> SOURCE_POOL; // source index name, is used

	SOURCE_POOL m_akSourcePool[SOURCE_POOL_SIZE];

	map<string, ZFResourceHandle*> m_mkResHandles;
	map<string, unsigned short> m_mkResHandleCounter;

	list<SoundInfo*> m_kActiveSounds;

	// Common
	bool GenerateSourcePool();
	ZFResourceHandle* GetResHandle(string strFileName);
	bool Hearable(SoundInfo* pkSound);
	bool RestartLoopSound(SoundInfo* pkSound);
	bool LoadRes(SoundInfo* pkSound);
	bool UnLoadRes(SoundInfo* pkSound);
	unsigned short ChangeResCounter(string strFileName, unsigned short modification);
};

#endif // #ifndef _THE_ZFAUDIOSYSTEM_H_
