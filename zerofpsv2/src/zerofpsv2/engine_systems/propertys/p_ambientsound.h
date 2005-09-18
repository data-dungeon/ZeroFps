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

		ZSSAudioSystem* m_pkAudioSystem;
		EntityManager* m_pEntityMan;
		Render* m_pkRender;
		ZeroFps* m_pkZeroFps;

		float m_fWidth;
		float m_fHeight;	

		Vector3 m_kPrevPos;
		string m_strPrevSound;

	public:

		P_AmbientSound();
		~P_AmbientSound();

		void Update();

		void Save(ZFIoInterface* pkFile);
		void Load(ZFIoInterface* pkFile,int iVersion);

		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );

		void SetArea(vector<Vector2>& kPolygon, float fFloor, float fRoof);
		void GetArea(vector<Vector2>& kPolygon, float& rfFloor, float& rfRoof);

		void SetSound(string strSound);
		string GetSound();

		void SetFreeForm(bool bFreeform);

	protected:

		int m_iAmbientAreaID;
		string m_strSound;
		bool m_bSoundStarted;
		vector<Vector2> m_kPolygon; // lokala koordinater från objektet
		float m_fFloor, m_fRoof; // lokala koordinater från objektet

		bool m_bFreeForm; // tillåt areor som inte är fyrkantiga

		vector<PropertyValues> GetPropertyValues();

		
};

Property* Create_AmbientSound();


#endif // #ifndef _ENGINE_P_AMBIENTSOUND_H_



