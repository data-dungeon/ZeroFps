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
#include "../../basic/matrix4.h"

#define DUMMY_SOUND "/data/sound/dummy.wav"
#define HEARABLE_DISTANCE 100.0f

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
	 \ingroup Engine

Use this system to play (start) or stop sounds by 
creating a (temporary) ZFSoundInfo object and call StartSound. Both loop sounds and
single sound can be created. The system creates internal copies of the sounds and uses
a cach system to load and unload wav files from memory when they are no longer needed,
Importent: Only mono wav files should be used as resources!
*/

class ENGINE_SYSTEMS_API ZFAudioSystem  : public ZFSubSystem 
{

public:
	void ReverbTest();

	void UnloadAll();

	bool SetVolume(float fVolume);
	float GetVolume() { return m_fVolume; }

	bool StartSound(string strName, Vector3 pos=m_kPos, Vector3 dir=Vector3(0,0,1), bool bLoop=false);
	bool StopSound(string strName, Vector3 pos);

	bool LoadSound(string strFileName);
	bool UnLoadSound(string strFileName);

	bool MoveSound(const char* szName, Vector3 kOldPos, Vector3 kNewPos, Vector3 kNewDir=Vector3(0,0,1));

	unsigned int GetNumSounds();
	unsigned int GetNumActiveChannels();
	unsigned int GetCurrentCachSize() { return m_uiCurrentCachSize; }

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
	void SetListnerPosition(Vector3 kPos,Matrix4 kOri);	
	Vector3 GetListnerPos() { return m_kPos;  }
	Vector3 GetListnerDir() { return m_kHead; }
	static void PrintError(ALenum error, char* szDesc);

	ZFAudioSystem(int uiMaxCachSize=19021844); // ca. 18 Meg
	virtual ~ZFAudioSystem();

private:

	unsigned int m_uiCurrentCachSize; // bytes
	unsigned int m_uiMaxCachSize; // bytes
	
	int m_iEnableSound; // enable/disable

	bool m_bIsValid;
	float m_fVolume;
	
	OggMusic* m_pkMusic;

	static Vector3 m_kPos;
	Vector3 m_kHead;
	Vector4 m_kUp;

	typedef pair<ZFResourceHandle*, int> tResPriorHandle;
	
	map<string, tResPriorHandle> m_mkResHandles;
	
	list<ZFSoundInfo*> m_kSoundList;

	// Common
	ZFResourceHandle* GetResHandle(string strFileName);
	bool Hearable(ZFSoundInfo* pkSound);
	ZFSoundInfo* GetClosest(const char* szName, Vector3 kPos);
	bool ResourceIsUnused(ZFSoundInfo* pkSound);
	bool InitSound(ZFSoundInfo* pkSound);
	bool Restart(ZFSoundInfo *pkSound);
	void Stop(ZFSoundInfo* pkSound);
	bool Play(ZFSoundInfo* pkSound);
	bool DeleteSound(ZFSoundInfo* pkSound, bool bRemoveFromSystem);
	void GetSoundsUsingResource(ZFSoundRes* pkRes, vector<ZFSoundInfo*>& vkSounds);
	int ModifyResHandlePriority(string strFileName, int mod);
	int GetResHandlePriority(string strFileName);
	bool GetSoundWithLowestPriority(string& strRes);
};

#endif // #ifndef _THE_ZFAUDIOSYSTEM_H_
