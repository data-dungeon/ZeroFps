// zfaudiosystem.h

#ifndef _THE_ZFAUDIOSYSTEM_H_
#define _THE_ZFAUDIOSYSTEM_H_

#pragma warning( disable : 4512) // 'class' : assignment operator could not be generated 

#include <map>
#include <list>
#include "../engine_systems_x.h"
#include "../../basic/zfresource.h"
#include "../../basic/zfsubsystem.h"
#include "../../basic/vector2.h"
#include "../../basic/vector3.h"
#include "../../basic/vector4.h"
#include "../../basic/matrix4.h"
#include "../../basic/matrix3.h"
#include "oggstream.h"

#define DUMMY_SOUND "/data/sound/dummy.wav"
#define HEARABLE_DISTANCE 100.0f

// PlayAudio options 
#define ZFAUDIO_LOOP			1  // loopa ljudet när det har spelat klart
#define ZFAUDIO_3DOGG		2	// ogg:en har en position i 3d världen OBS! filen måste vara mono
#define ZFAUDIO_2D			4  // en wav fil skall spelas som ett vanligt 2d ljud

class ZFAudioSystem;
class EntityManager;

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
		bool m_b2DSound; // normalt 2d ljud som inte har någon position
		Vector3 m_kPos;
		Vector3 m_kDir;
		char m_acFile[128]; // ändra inte till string, då uppstår en kopiering bugg!!

		long m_iID;
		float m_fGain;
			
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

	ALuint GetNewSource();

	float GetMainVolume() { return m_fMainVolume; }
	bool SetMainVolume(float fVolume) 
	{ 
		if(fVolume >= 0 && fVolume <= 1) 
		{
			m_fMainVolume = fVolume; 
			return true;
		}
		return false;
	}

	//
	// Ambient sound area
	//

	int AddAmbientArea(string strName, vector<Vector2>& kArea);
	void RemoveAmbientArea(int iID);
	bool ChangePntsInAmbientArea(int iID, vector<Vector2>& kArea);
	bool ChangeAmbientAreaSound(int iID, string strSound);
	
	//
	// Audiosystem
	//

	void UnloadAll();

	bool SetSoundVolume(float fVolume);
	float GetSoundVolume() { return m_fSoundVolume; }

	int PlayAudio(string strName, Vector3 kPos=m_kPos, Vector3 kDir=Vector3(0,0,1), int flags=0, float fGain=1.0f);
	bool StopAudio(int iID, bool bRelease=true);
	bool StopAudio(string strName, Vector3 pos, bool bRelease=true);	
	bool MoveAudio(int iID, Vector3 kNewPos, Vector3 kNewDir = Vector3(0,0,1), float fVolume = -1 ); // if fVolume is -1 then dont change volume
	
	bool LoadSound(string strFileName);
	bool UnLoadSound(string strFileName);

	bool SetSoundGain(int iID, float fGain);

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
		FID_AUDIOPLAY,
		FID_AUDIOSTOP,
	};

	void RunCommand(int cmdid, const CmdArgument* kCommand);

	//
	// Common
	//
	void Update();
	void SetListnerPosition(Vector3 kPos,Vector3 kHead,Vector3 kUp);
	void SetListnerPosition(Vector3 kPos,Matrix4 kOri);
	void SetListnerPosition(Vector3 kPos,Matrix3 kOri);
	void SetReferensDistance(float fR) {m_fReferenceDistance = fR;};
	static Vector3 GetListnerPos() { return m_kPos;  }
	Vector3 GetListnerDir() { return m_kHead; }
	static void PrintError(ALenum error, char* szDesc);
	
	ZFAudioSystem(int uiMaxCachSize=19021844); // ca. 18 Meg
	virtual ~ZFAudioSystem();

private:

	float m_fMainVolume; // 0-1

	unsigned int m_uiCurrentCachSize; // bytes
	unsigned int m_uiMaxCachSize; // bytes
	
	bool m_bEnableSound; // enable/disable sound
	bool m_bEnableMusic; // enable/disable music
	float m_fReferenceDistance; // ReferenceDistance (ljud styrka)

	bool m_bIsValid;
	float m_fSoundVolume, m_fMusicVolume;
	
	static Vector3 m_kPos;
	Vector3 m_kHead;
	Vector4 m_kUp;

	typedef pair<ZFResourceHandle*, int> tResPriorHandle;
	
	map<string, tResPriorHandle> m_mkResHandles;
	
	list<ZFSoundInfo*> m_kSoundList;

	map<int,ZFSoundInfo*> m_kLoopSoundMap; // En map som används för att slippa
														// söka igenom listan varje gång ett 
														// loopat ljud skall flyttas

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

	//
	// Ambient sound area
	//

	EntityManager* m_pEntityMan;

	class AmbientArea
	{
	public:

		AmbientArea();
		~AmbientArea();

		bool m_bChangeSound;
		float m_fGain;
		int m_iSoundID;
		string m_strSound;
		float m_fFadeTimer;
		vector<Vector2*> m_kPolygon;	

		int m_iAmbientAreaID;
	};

	vector<AmbientArea*> m_kAmbientAreas;
	
	inline int IsLeft( Vector2 *P0, Vector2 *P1, Vector2 *P2 ) 
	{
		return ( (P1->x - P0->x) * (P2->y - P0->y) - (P2->x - P0->x) * (P1->y - P0->y) );
	}

	bool PntInPolygon(Vector2 *pt, vector<Vector2*>& vPolygon);
	void FadeGain(AmbientArea* pkArea, bool bOut);
	void UpdateAmbientSound();

	vector<OggStream*> m_vkOggStreams;

};

#endif // #ifndef _THE_ZFAUDIOSYSTEM_H_
