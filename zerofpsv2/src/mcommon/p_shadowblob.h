#ifndef _P_SHADOWBLOB_H_
#define _P_SHADOWBLOB_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include "../zerofpsv2/render/zshadersystem.h"
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
		
		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName ,string kValue );			
		
		//ZFResourceHandle* m_pkTexture;
		
		bool		m_bHaveSet;
		Vector3	m_kOffset;
		Vector3	m_kScale;
	
		ZMaterial*	m_pkMaterial;
		
	public:

		void SetScale (float fWidth, float fLenght);
		
		P_ShadowBlob();
		~P_ShadowBlob();

		
		void Update();
		void Init();
		
		
			
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		

};

MCOMMON_API Property* Create_P_ShadowBlob();

#endif







