#ifndef _P_ENVIROMENT_H_
#define _P_ENVIROMENT_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <string>
#include <vector>


#include "../zerofpsv2/engine_systems/propertys/p_psystem.h"
#include "../zerofpsv2/engine_systems/propertys/p_light.h"
#include "../zerofpsv2/engine_systems/propertys/p_skyboxrender.h"

#include "zssenviroment.h"

using namespace std;


class ZSSEnviroment;
class EnvSetting;

class MCOMMON_API P_Enviroment: public Property {
	private:
		ZeroFps* 		m_pkFps;
		EntityManager* m_pkEntityManager;
		Render*			m_pkRender;
		ZShaderSystem*	m_pkZShaderSystem;
		ZSSEnviroment*	m_pkEnviroment;
		ZFAudioSystem*	m_pkAudioSystem;
		
		bool				m_bEnabled;		
		string			m_strCurrentZoneEnviroment;
		string			m_strLastSetEnviroment;
				
		EnvSetting*		m_pkZoneEnvSetting;
		EnvSetting		m_kCurrentEnvSetting;
		
		
		void UpdateEnviroment();
						
	public:

		P_Enviroment();
		~P_Enviroment();
		
		void Init();
		void Update();
		void ZoneChange(int iCurrent,int iNew);

		void UpdateEnvSetting(EnvSetting* pkEnvSetting);
		
		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );

		void SetEnable(bool bNew);
		
		friend class ZSSEnviroment;
};

MCOMMON_API Property* Create_P_Enviroment();



		/*
		void SetEnviroment(const char* csEnviroment);
		void ResetEnviroment();
		*/
		
		/*
		int				m_iMusicHandle;
		
		float				m_fTimer;
		float				m_fRainUpdateTimer;
		
				
		string			m_StrCurrentEnviroment;
		
		
		P_Light*			m_pkCurrentLP;
		
		Vector4			m_kSunDiffuseColor;
		Vector4			m_kSunAmbientColor;		
		Vector3			m_kSunPos;
		
		Vector4			m_kFogColor;
		float				m_fFogStart;					
		float				m_fFogStop;					
		
		Vector4			m_kCurrentFogColor;
		float				m_fCurrentFogStart;					
		float				m_fCurrentFogStop;					

		
		ZMaterial*		m_pkSunMat;
		ZMaterial*		m_pkSunFlareMat;
				
		int				m_iRainTextureID;
		int				m_iRain;
		
		vector<Vector3> m_kDrops;
		void MakeRainSplashes();
		void DrawRainSplashes();		

		void DrawSun();
		*/

#endif









