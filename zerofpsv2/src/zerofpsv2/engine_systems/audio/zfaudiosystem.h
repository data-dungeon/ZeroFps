// zfaudiosystem.h

#ifndef _THE_ZFAUDIOSYSTEM_H_
#define _THE_ZFAUDIOSYSTEM_H_

#pragma warning( disable : 4512) // 'class' : assignment operator could not be generated 

#include <map>
#include <list>
#include "../engine_systems_x.h"
#include "../../basic/zfresource.h"
#include "../../basic/zfsubsystem.h"
#include "../../basic/math.h"
#include "../../basic/concommand.h"
#include "oggstream.h"

#define DUMMY_SOUND "/data/audio/dummy.wav"
#define HEARABLE_DISTANCE 25.0f

// PlayAudio options 
#define ZFAUDIO_LOOP			1  // loopa ljudet n�r det har spelat klart
#define ZFAUDIO_3DOGG		2	// ogg:en har en position i 3d v�rlden OBS! filen m�ste vara mono
#define ZFAUDIO_2D			4  // en wav fil skall spelas som ett vanligt 2d ljud

class ZSSAudioSystem;
class ZSSEntityManager;

/// A Sound in the Resource SubSystem.
class ENGINE_SYSTEMS_API ZFSoundRes : public ZFResource
{
	public:
		ALuint GetBufferIndexName();
		ZFSoundRes();
		~ZFSoundRes();
		bool Create(const string& strName);	// overloaded
		int CalculateSize();
		bool IsValid()	const						{	return m_uiBufferIndexName != 0;	};
		
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
		bool m_b2DSound; // normalt 2d ljud som inte har n�gon position
		Vector3 m_kPos;
		Vector3 m_kDir;
		char m_acFile[128]; // �ndra inte till string, d� uppst�r en kopiering bugg!!

		long m_iID;
		float m_fGain;
			
	private:
		ZFSoundRes* m_pkResource;
		ALuint m_uiSourceBufferName;
		bool m_bLoopingNoLongerHearable;

		friend class ZSSAudioSystem;
};

/** \brief	Sub System for Sound. 
	 \ingroup Engine

Use this system to play (start) or stop sounds by 
creating a (temporary) ZFSoundInfo object and call StartSound. Both loop sounds and
single sound can be created. The system creates internal copies of the sounds and uses
a cach system to load and unload wav files from memory when they are no longer needed,
Importent: Only mono wav files should be used as resources!
*/

class ENGINE_SYSTEMS_API ZSSAudioSystem  : public ZFSubSystem
{

public:

	ALuint GetNewSource();

	float GetMainVolume() { return m_kfMainVolume.GetFloat(); }
	bool SetMainVolume(float fVolume) 
	{ 
		if(fVolume >= 0 && fVolume <= 1) 
		{
			m_kfMainVolume.SetFloat(fVolume); 
			return true;
		}
		return false;
	}

	//
	// Ambient sound area
	//

	int AddAmbientArea(string strName, vector<Vector2>& kArea, float fFloor, float fRoof);
	void RemoveAmbientArea(int iID);
	bool ChangePntsInAmbientArea(int iID, vector<Vector2>& kArea);
	bool ChangeAmbientAreaYPos(int iID, float fFloor, float fRoof);
	bool ChangeAmbientAreaSound(int iID, string strSound);
	
	//
	// Audiosystem
	//

	void UnloadAll();

	void SetMusic(const string& strMusic);

	int PlayAudio(string strName, Vector3 kPos=m_kPos, Vector3 kDir=Vector3(0,0,1), int flags=0, float fGain=1.0f);
	bool StopAudio(int iID, bool bRelease=true);
	bool StopAudio(string strName, Vector3 pos, bool bRelease=true);	
	bool MoveAudio(int iID, Vector3 kNewPos, Vector3 kNewDir = Vector3(0,0,1), float fVolume = -1 ); // if fVolume is -1 then dont change volume
	
	bool LoadSound(string strFileName);
	bool UnLoadSound(string strFileName);

	bool SetGain(int iID, float fGain);

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

	void RunCommand(int cmdid, const ConCommandLine* kCommand);

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
	
	ZSSAudioSystem(int uiMaxCachSize=19021844); // ca. 18 Meg
	virtual ~ZSSAudioSystem();

private:

	ALCdevice*	m_pkAudioDevice;
	ALCcontext*	m_pkAudioContextID;

	ConVar m_kfMainVolume; // 0-1
	
	unsigned int m_uiCurrentCachSize; // bytes
	unsigned int m_uiMaxCachSize; // bytes
	
	float m_fReferenceDistance; // ReferenceDistance (ljud styrka)
	bool m_bIsValid;
	
	
	//music
	int		m_iMusicID;
	float		m_fMusicFade;
	string	m_strCurrentMusic;
	string	m_strNextMusic;
	bool		m_bMusicChange;
	ConVar	m_kfMusicGain;
	
	static Vector3 m_kPos;
	Vector3 m_kHead;
	Vector4 m_kUp;

	typedef pair<ZFResourceHandle*, int> tResPriorHandle;
	
	map<string, tResPriorHandle> m_mkResHandles;
	
	list<ZFSoundInfo*> m_kSoundList;

	map<int,ZFSoundInfo*> m_kLoopSoundMap; // En map som anv�nds f�r att slippa
														// s�ka igenom listan varje g�ng ett 
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

	//  Music
	void UpdateMusic();

	//
	// Ambient sound area
	//

	ZSSEntityManager* m_pEntityMan;
	ZSSZeroFps* 		m_pkZeroFps;

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
		float m_fFloor, m_fRoof;

		int m_iAmbientAreaID;
	};

	vector<AmbientArea*> m_kAmbientAreas;
	
	inline int IsLeft( Vector2 *P0, Vector2 *P1, Vector2 *P2 ) 
	{
		return int( (P1->x - P0->x) * (P2->y - P0->y) - (P2->x - P0->x) * (P1->y - P0->y) );
	}

	bool PntInPolygon(Vector2 *pt, vector<Vector2*>& vPolygon);
	void FadeGain(AmbientArea* pkArea, bool bOut);
	void UpdateAmbientSound();

	vector<OggStream*> m_vkOggStreams;

};

#endif // #ifndef _THE_ZFAUDIOSYSTEM_H_
