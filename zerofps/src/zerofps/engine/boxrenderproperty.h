#ifndef _BOXRENDERPROPERTY_H_
#define _BOXRENDERPROPERTY_H_

#include "../basic/basic.pkg"
#include "../render/render.pkg"
#include "../engine/engine.pkg"
#include "property.h"
#include <iostream>
#include <string>

using namespace std;


class ENGINE_API BoxRenderProperty: public Property {
		Frustum* m_pkFrustum;
		TextureManager* m_pkTexMan;
		Render* m_pkRender;	
	
		string m_kTex1; 

		int m_iTex1;
		Vector3 m_kScale;
		
		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName ,string kValue );		
	
	
		float GetBoundingRadius();	
		
	public:
		BoxRenderProperty();
		void Update();

		void SetTexture(const char* acTex1);
		void SetScale(Vector3 kScale) {m_kScale=kScale;};
		void UpdateSet();
		
		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);
		
};

Property* Create_BoxRenderProperty();

#endif

