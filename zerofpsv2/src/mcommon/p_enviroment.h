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

#include "../zerofpsv2/basic/zfresource.h"
#include "../zerofpsv2/basic/zfini.h"

#include "../zerofpsv2/engine_systems/propertys/p_psystem.h"
#include "../zerofpsv2/engine_systems/propertys/p_light.h"
#include "../zerofpsv2/engine_systems/propertys/p_skyboxrender.h"

using namespace std;

/**	\brief	Da EnvSetting
		\ingroup Common
*/

class MCOMMON_API EnvSetting : public ZFResource
{
	private:
		ZFIni		m_kIni;
	
		string	m_strMusic;
		string	m_strParticles;
		string	m_strSkybox1;		
		string	m_strSkybox2;				
		
		Vector4	m_kSunDiffuseColor;
		Vector4	m_kSunAmbientColor;		
		Vector3	m_kSunPos;
		
		float		m_fFogStart;
		float		m_fFogStop;		
		Vector4	m_kFogColor;
		
		int 		m_iRain;
	
	public:
		EnvSetting();
		~EnvSetting();	
	
		bool LoadEnviroment(const char* czName);
		void Clear();
		bool Create(string strName);	//	for resource system
		int  CalculateSize();		

	friend class P_Enviroment;
};

MCOMMON_API ZFResource* Create__EnvSetting();

//---end of EnvSetting


class MCOMMON_API P_Enviroment: public Property {
	private:
		ZeroFps* 		m_pkFps;
		EntityManager* m_pkEntityManager;
		Render*			m_pkRender;
		ZShaderSystem*	m_pkZShaderSystem;
		OggMusic*		m_pkMusic;		
		
		float				m_fTimer;
		float				m_fRainUpdateTimer;
		
		
		bool				m_bEnabled;		
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
				
	public:

		P_Enviroment();
		~P_Enviroment();
		
		void Init();
		void Update();
		void ZoneChange(int iCurrent,int iNew);

		void SetEnviroment(const char* csEnviroment);
		void ResetEnviroment();

		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );

		void SetEnable(bool bNew);
};

MCOMMON_API Property* Create_P_Enviroment();

#endif
