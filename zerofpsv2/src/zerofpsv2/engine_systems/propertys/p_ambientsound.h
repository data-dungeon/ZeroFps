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

		// Clientdata
		bool m_bChangeSound; // bara på klienten
		float m_fGain;
		int m_iSoundID;
		string m_strSound;
		float m_fFadeTimer;

		void FadeGain(bool bOut);

		// Serverdata
		int m_iCurrentAmbientArea;
		bool m_bDotFileLoaded;
		bool m_bDotFileDontExist;

		bool LoadDotFile(string strFileName);
		bool PntInPolygon(Vector2 *pt, vector<Vector2*>& vPolygon);

		inline int IsLeft( Vector2 *P0, Vector2 *P1, Vector2 *P2 ) 
		{
			return ( (P1->x - P0->x) * (P2->y - P0->y) - (P2->x - P0->x) * (P1->y - P0->y) );
		}

		struct DOTFILE_INFO
		{
			string m_strAreaName;
			string m_strFileName;
			vector<Vector2*> m_kPolygon;	
		};

		vector<DOTFILE_INFO> m_kDotArray;

	public:

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



