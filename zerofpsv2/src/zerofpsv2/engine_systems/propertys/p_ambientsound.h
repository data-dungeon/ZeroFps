#ifndef _ENGINE_P_AMBIENTSOUND_H_
#define _ENGINE_P_AMBIENTSOUND_H_

#include "../../engine/property.h"
#include "../engine_systems_x.h"
#include "../audio/zfaudiosystem.h"
#include "../../engine/entitymanager.h"

using namespace std;

class ENGINE_SYSTEMS_API P_AmbientSound : public Property 
{
	private:

		ZFAudioSystem* m_pkAudioSystem;
		EntityManager* m_pEntityMan;

	public:

		P_AmbientSound();
		~P_AmbientSound();

		void Update();

		void Save(ZFIoInterface* pkFile);
		void Load(ZFIoInterface* pkFile,int iVersion);

		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );

		void SetArea(vector<Vector2>& kPolygon);
		void GetArea(vector<Vector2>& kPolygon);

		void SetSound(string strSound);
		string GetSound();

	protected:

		int m_iAmbientAreaID;
		string m_strSound;
		bool m_bSoundStarted;
		vector<Vector2> m_kPolygon;

		vector<PropertyValues> GetPropertyValues();

		Vector3 m_kPrevPos;
};

Property* Create_AmbientSound();


#endif // #ifndef _ENGINE_P_AMBIENTSOUND_H_



