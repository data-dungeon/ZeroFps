#ifndef _ENGINE_P_SOUND_H_
#define _ENGINE_P_SOUND_H_

#include "../../engine/property.h"
#include "../engine_systems_x.h"
#include "../audio/zfaudiosystem.h"
#include "../../engine/entitymanager.h"

using namespace std;

Property* Create_SoundProperty();

class ENGINE_SYSTEMS_API P_Sound : public Property
{
private:
	ZFAudioSystem* m_pkAudioSystem;
	EntityManager* m_pEntityMan;

	Vector3 m_kPrevpos;

	struct sound_info
	{
		string m_strFileName;
		bool m_bLoop;
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










/*/////////////// OLD Code (do not remove jet)

class ENGINE_SYSTEMS_API P_Sound : public Property
{
private:
	ZFAudioSystem* m_pkAudioSystem;
	EntityManager* m_pEntityMan;

	string m_strFileName;
	bool m_bLoop;
	
	string m_strStopFileName;

	bool m_bStarted;
	Vector3 m_kPrevpos;

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
*/

#endif // #ifndef _ENGINE_P_SOUND_H_