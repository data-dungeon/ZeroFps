// zfaudiosystem2.h: interface for the ZFAudioSystem2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZFAUDIOSYSTEM2_H__C102D4BD_588E_4554_AE5F_D84BC72D8D63__INCLUDED_)
#define AFX_ZFAUDIOSYSTEM2_H__C102D4BD_588E_4554_AE5F_D84BC72D8D63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

class ZFAudioSystem2;

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
		~SoundInfo();

		bool m_bLoop;
		Vector3 m_kPos;
		Vector3 m_kDir;
		char m_szFile[128]; // ändra inte till string, då uppstår en kopiering bugg!!
			
	private:
		ZFSoundRes* m_pkResource;
		ALuint m_uiSourceBufferName;

		friend ZFAudioSystem2;
};

class ENGINE_SYSTEMS_API ZFAudioSystem2  : public ZFSubSystem 
{
public:
	unsigned int GetNumActiveSounds();
	bool StopSound(SoundInfo kSound);
	bool ActivateSound(SoundInfo kSound);

	ZFAudioSystem2();
	virtual ~ZFAudioSystem2();

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

private:
	OggMusic* m_pkMusic;

	Vector3 m_kPos, m_kHead;
	Vector4 m_kUp;
	
	typedef pair<ALuint, bool> SOURCE_POOL; // source index name, is used

	SOURCE_POOL m_akSourcePool[SOURCE_POOL_SIZE];

	map<string, ZFResourceHandle*> m_mkResHandles;

	list<SoundInfo*> m_kActiveSounds;

	// Common
	bool GenerateSourcePool();
	ZFResourceHandle* GetResHandle(string strFileName);
	bool Hearable(SoundInfo* pkSound);

};



#endif // !defined(AFX_ZFAUDIOSYSTEM2_H__C102D4BD_588E_4554_AE5F_D84BC72D8D63__INCLUDED_)
