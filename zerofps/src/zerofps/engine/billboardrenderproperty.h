#ifndef _ENGINE_BILLBOARDRENDERPROPERTY_H_
#define _ENGINE_BILLBOARDRENDERPROPERTY_H_

#include "../basic/basic.pkg"
#include "../render/render.pkg"
#include "../engine/engine.pkg"
#include "property.h"
#include <iostream>
#include <string>

using namespace std;


class ENGINE_API BillBoardRenderProperty: public Property {
		Frustum* m_pkFrustum;
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
		
		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);
		
};

Property* Create_BillBoardRenderProperty();

#endif

