#ifndef _ENGINE_P_SOUND_H_
#define _ENGINE_P_SOUND_H_

#include "../../engine/property.h"
#include "../engine_systems_x.h"
#include "../audio/zfaudiosystem.h"
#include "../../engine/zerofps.h"

using namespace std;

enum SoundAction
{
	SA_DO_NOTHING,
	SA_START_SOUND, 
	SA_STOP_SOUND
};

class ENGINE_SYSTEMS_API P_Sound : public Property
{
private:
	ZFAudioSystem* m_pkAudioSystem;
	ZeroFps* m_pkFps;

	struct sound_info
	{
		string m_strFileName;
		bool m_bLoop;
		bool m_bPlaying;

		SoundAction m_eAction;
	};

	vector<sound_info> m_kSounds;

public:
	P_Sound();
	~P_Sound();

	void StartSound(string strName, bool bLoop);
	void StopSound(string strName);

	void Update();
	void PackTo(NetPacket* pkNetPacket, int iConnectionID);
	void PackFrom(NetPacket* pkNetPacket, int iConnectionID);

	void Save(ZFIoInterface* pkFile);
	void Load(ZFIoInterface* pkFile,int iVersion);

	vector<PropertyValues> GetPropertyValues();
};

#endif // #ifndef _ENGINE_P_SOUND_H_