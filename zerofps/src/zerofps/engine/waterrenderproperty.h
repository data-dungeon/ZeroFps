#ifndef _ENGINE_WATERRENDERPROPERTY_H_
#define _ENGINE_WATERRENDERPROPERTY_H_

#include "../basic/basic.pkg"
#include "../render/render.pkg"
#include "../engine/engine.pkg"
#include "property.h"
#include <iostream>

#include <string.h>
//#include "GL/glut.h"

using namespace std;


class ENGINE_API WaterRenderProperty: public Property {
		TextureManager* m_pkTexMan;
		ZeroFps* m_pkZeroFps;
		Render* m_pkRender;	
	
		int m_iTexture;		
		string m_sTexture;
	
		int m_iSize;
		int m_iStep;
		
		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName ,string kValue );		
		
	public:
		WaterRenderProperty();
		void CloneOf(Property* pkProperty);

		void SetProperty(int iSize,int iStep,const char* acTexture);
		void SetTexture(const char* acTexture);
		void Update();

		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);


};

ENGINE_API Property* Create_WaterRenderProperty();


#endif

