#ifndef _P_SHADOWBLOB_H_
#define _P_SHADOWBLOB_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include "../zerofpsv2/render/zshadersystem.h"
#include "../zerofpsv2/engine_systems/tcs/tcs.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <string>
#include <vector>

using namespace std;

class MCOMMON_API P_ShadowBlob: public Property {
	private:
		//Render*	m_pkRender;
		ZShaderSystem*	m_pkZShaderSystem;
		Tcs*				m_pkTcs;
		
		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName ,string kValue );			
		
		//ZFResourceHandle* m_pkTexture;
		
		bool		m_bHaveSet;
		Vector3	m_kOffset;
		float		m_fScale;
	
		ZMaterial*	m_pkMaterial;
		
		
		Vector3 GetShadowPos();
		
	public:

		void SetScale (float fWidth, float fLenght);
		
		P_ShadowBlob();
		~P_ShadowBlob();

		
		void Update();
		void Init();
		
		void PackTo(NetPacket* pkNetPacket, int iConnectionID ) ;
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID ) ;		
			
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		

};

MCOMMON_API Property* Create_P_ShadowBlob();

#endif







