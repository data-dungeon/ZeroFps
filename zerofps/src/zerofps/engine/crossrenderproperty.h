#ifndef _CROSSRENDERPROPERTY_H_
#define _CROSSRENDERPROPERTY_H_

#include "../basic/basic.pkg"
#include "../render/render.pkg"
#include "../engine/engine.pkg"
#include "property.h"
#include <iostream>
#include <string>

using namespace std;


class ENGINE_API CrossRenderProperty: public Property {
		Frustum* m_pkFrustum;
		TextureManager* m_pkTexMan;
		Render* m_pkRender;	
	
		string m_kTex1; 
//		string m_kTex2;		
		
		int m_iTex1;
//		int m_iTex2;
	
		Vector3 m_kScale;
		
		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName ,string kValue );		
	
	public:
		CrossRenderProperty();
		void Update();

		void SetTexture(const char* acTex1);//,const char* acTex2);// {m_kTex1=acTex1 ; m_kTex2=acTex2;};
		void SetScale(Vector3 kScale) {m_kScale=kScale;};
		void UpdateSet();
		
		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);
		
};

Property* Create_CrossRenderProperty();

#endif

