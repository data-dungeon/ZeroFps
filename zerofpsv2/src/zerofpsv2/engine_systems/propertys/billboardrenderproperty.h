#ifndef _ENGINE_BILLBOARDRENDERPROPERTY_H_
#define _ENGINE_BILLBOARDRENDERPROPERTY_H_

#include "../../engine/property.h"
#include "../../basic/zfobjectmanger.h"
#include <iostream>
#include <string>
#include "../engine_systems_x.h"

using namespace std;


class ENGINE_SYSTEMS_API BillBoardRenderProperty: public Property {
		TextureManager* m_pkTexMan;
		Render* m_pkRender;	
		ZeroFps* m_pkFps;
			
		string m_kTexture; 
		
		int m_iTexture; 
		Vector3 m_kAxis;	
		float m_fScale;
		
		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName ,string kValue );		
	
	public:
		BillBoardRenderProperty();
		void CloneOf(Property* pkProperty) { }
		void Update();

		void SetTexture(const char* acTexure);
		void SetScale(float fScale) {m_fScale=fScale;};
		void UpdateSet();
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);
		
};

Property* Create_BillBoardRenderProperty();

#endif

