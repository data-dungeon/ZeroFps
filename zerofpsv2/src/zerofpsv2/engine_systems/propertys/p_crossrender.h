#ifndef _ENGINE_CROSSRENDERPROPERTY_H_
#define _ENGINE_CROSSRENDERPROPERTY_H_

#include "../../engine/property.h"
#include <iostream>
#include <string>
#include "../engine_systems_x.h"
#include "../../engine/zerofps.h"

using namespace std;


class ENGINE_SYSTEMS_API P_CrossRender : public Property {

		TextureManager* m_pkTexMan;
		Render* m_pkRender;	
		ZeroFps* m_pkFps;
		string m_kTex1; 
		
		int m_iTex1;
	
		Vector3 m_kScale;
		
		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName ,string kValue );		
	
	public:
		P_CrossRender();
		void Update();

		void SetTexture(const char* acTex1);//,const char* acTex2);// {m_kTex1=acTex1 ; m_kTex2=acTex2;};
		void SetScale(Vector3 kScale) {m_kScale=kScale;};
		void UpdateSet();
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		
};

Property* Create_CrossRenderProperty();

#endif

