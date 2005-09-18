#ifndef _ENGINE_P_SOUND_H_
#define _ENGINE_P_SOUND_H_
 
#include "../../engine/property.h"
#include "../engine_systems_x.h"
#include "../audio/zfaudiosystem.h"
#include "../../engine/entitymanager.h"
#include "../../engine/zerofps.h"


ENGINE_SYSTEMS_API Property* Create_SoundProperty();
ENGINE_SYSTEMS_API void Register_P_Sound(ZeroFps* pkZeroFps);


class ENGINE_SYSTEMS_API P_Sound : public Property
{
private:
	ZSSAudioSystem* m_pkAudioSystem;
	EntityManager* m_pEntityMan;

	string m_strFileName;
	bool m_bLoop;
	float m_fGain;
	float m_fStartTime;
	
	string m_strStopFileName;

	bool m_bStarted;
	Vector3 m_kPrevpos;
	int m_iID;

public:
	P_Sound();
	~P_Sound();

	void StartSound(string strName, bool bLoop, float fGain=1.0f);
	void StopSound(string strName);

	void Update();
	void PackTo(NetPacket* pkNetPacket, int iConnectionID);
	void PackFrom(NetPacket* pkNetPacket, int iConnectionID);

	void Save(ZFIoInterface* pkFile);
	void Load(ZFIoInterface* pkFile,int iVersion);

	bool HandleSetValue( const string& kValueName ,const string& kValue );

	vector<PropertyValues> GetPropertyValues();

};

/*
class ENGINE_SYSTEMS_API P_Sound : public Property
{
private:
	ZSSAudioSystem* m_pkAudioSystem;
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








