#ifndef _ENGINE_P_AMBIENTSOUND_H_
#define _ENGINE_P_AMBIENTSOUND_H_

#include "../../engine/property.h"
#include "../engine_systems_x.h"
#include "../audio/zfaudiosystem.h"
#include "../../engine/zerofps.h"

using namespace std;

class ENGINE_SYSTEMS_API P_Sound : public Property
{
private:
	ZFAudioSystem* m_pkAudioSystem;
	ZeroFps* m_pkFps;
	string m_strFileName, m_strFileNameClient;

public:
	P_Sound();
	~P_Sound();

	void Play(string strName);
	void Update();
	void PackTo(NetPacket* pkNetPacket, int iConnectionID);
	void PackFrom(NetPacket* pkNetPacket, int iConnectionID);
};

Property* Create_SoundProperty();

class ENGINE_SYSTEMS_API P_AmbientSound : public Property 
{
	private:

		float m_fHearableDistance;
		//char* m_szFileName;
		string m_strFileName;
		bool m_bLoop;
		bool m_bStarted;
		bool m_bSoundHaveBeenSaved; ///< when the sound loads
		bool m_bManagedByAudioSystem; ///< sätt till true om du vill att ljudet 
												///< skall stoppas/ startas automatiskt av systemet

		ZFAudioSystem* m_pkAudioSystem;
		ZeroFps* m_pkFps;

	public:
		void Stop(); ///< OBS! Ifall detta property har flaggan m_bManagedByAudioSystem satt till true
						 ///< så måste ett anrop till denna funktion göras innan ett objektet förstörs, annars 
						 ///< forstätter ljudet att spelas upp! Detta eftersom ljud i normala fall skall
						 ///< fortsätta spelas (hanteras av audio systmet) efter det att zonsystemet har 
						 ///< tagit bort objektet.

		bool SetSound(char* szFileName, bool bPlayOnes=false, float fHearableDistance=90.0f);

		P_AmbientSound();
		~P_AmbientSound();

		void Update();

		void Save(ZFIoInterface* pkFile);
		void Load(ZFIoInterface* pkFile,int iVersion);

		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );

	protected:

		vector<PropertyValues> GetPropertyValues();

		

};

Property* Create_AmbientSound();


#endif // #ifndef _ENGINE_P_AMBIENTSOUND_H_



