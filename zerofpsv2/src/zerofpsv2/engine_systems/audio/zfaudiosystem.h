// zfaudiosystem.h

#ifndef _THE_ZFAUDIOSYSTEM_H_
#define _THE_ZFAUDIOSYSTEM_H_

#include <map>
#include <list>
#include "../engine_systems_x.h"
#include "../../basic/zfresource.h"
#include "../../basic/zfsubsystem.h"
#include "../../basic/vector3.h"
#include "../../basic/vector4.h"
#include "oggmusic.h"

#define DUMMY_SOUND "/data/sound/dummy.wav"

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
		unsigned int m_uiSize;
};

ENGINE_SYSTEMS_API ZFResource* Create__WavSound();

class ENGINE_SYSTEMS_API ZFSoundInfo
{
	public:
		ZFSoundInfo();
		ZFSoundInfo(const char* c_szFile, Vector3 pos, Vector3 dir, bool bLoop=false);
		~ZFSoundInfo();

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

/** \brief	Sub System for Sound. 

Use this system to play (start) or stop sounds by 
creating a (temporary) ZFSoundInfo object and call StartSound. Both loop sounds and
single sound can be created. The system creates internal copies of the sounds and uses
a cach system to load and unload wav files from memory when they are no longer needed,
Importent: Only mono wav files should be used as resources!
*/

class ENGINE_SYSTEMS_API ZFAudioSystem  : public ZFSubSystem 
{
public:
	
	bool StartSound(ZFSoundInfo kSound);
	bool RemoveSound(ZFSoundInfo kSound, float fMaxSearchRange = 1000000.0f);

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

	unsigned int m_uiSourcePoolSize;
	SOURCE_POOL* m_pkSourcePool;

	map<string, ZFResourceHandle*> m_mkResHandles;
	map<string, unsigned short> m_mkResHandleCounter;

	list<ZFSoundInfo*> m_kSoundList;

	// Common
	bool GenerateSourcePool();
	ZFResourceHandle* GetResHandle(string strFileName);
	bool Hearable(ZFSoundInfo* pkSound);
	bool RestartLoopSound(ZFSoundInfo* pkSound);
	bool LoadRes(ZFSoundInfo* pkSound);
	bool UnLoadRes(ZFSoundInfo* pkSound);
	unsigned short ChangeResCounter(string strFileName, unsigned short modification);
	int GetFreeChannel();
};

#endif // #ifndef _THE_ZFAUDIOSYSTEM_H_
