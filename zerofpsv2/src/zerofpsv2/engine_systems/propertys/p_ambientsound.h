#ifndef _ENGINE_P_AMBIENTSOUND_H_
#define _ENGINE_P_AMBIENTSOUND_H_

#include "../../engine/property.h"
#include "../engine_systems_x.h"
#include "../audio/zfaudiosystem.h"

using namespace std;

class ENGINE_SYSTEMS_API P_AmbientSound : public Property 
{
	private:

		float m_fHearableDistance;
		//char* m_szFileName;
		string m_strFileName;
		bool m_bLoop;
		bool m_bStarted;
		bool m_bSoundHaveBeenSaved; ///< when the sound loads

		ZFAudioSystem* m_pkAudioSystem;

	public:
		bool SetSound(char* szFileName, bool bPlayOnes=false, float fHearableDistance=90.0f);

		P_AmbientSound();
		~P_AmbientSound();

		void CloneOf(Property* pkProperty);
		void Update();

		void Save(ZFIoInterface* pkFile);
		void Load(ZFIoInterface* pkFile);

	protected:

		vector<PropertyValues> GetPropertyValues();

		

};

Property* Create_AmbientSound();


#endif // #ifndef _ENGINE_P_AMBIENTSOUND_H_



