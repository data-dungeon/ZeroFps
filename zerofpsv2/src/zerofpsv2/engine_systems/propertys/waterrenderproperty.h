#ifndef _ENGINE_WATERRENDERPROPERTY_H_
#define _ENGINE_WATERRENDERPROPERTY_H_

#include "../engine_systems_x.h"
#include "../../engine/property.h"
#include <iostream>
#include "../../basic/zfobjectmanger.h"
#include <string.h>
#include "../../engine/zerofps.h"
#include "../../engine/objectmanager.h"

using namespace std;

/// Property to render a watersurface.
class ENGINE_SYSTEMS_API WaterRenderProperty: public Property {
		TextureManager* m_pkTexMan;
		ZeroFps* m_pkZeroFps;
		Render*	m_pkRender;	
	
		int		m_iTexture;		
		string	m_sTexture;
		int		m_iSize;
		int		m_iStep;

		float		m_fBlendValue;
		bool		m_bBlendDirUp;
		
		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName ,string kValue );		
		
	public:
		WaterRenderProperty();
		void CloneOf(Property* pkProperty);

		void SetProperty(int iSize,int iStep,const char* acTexture);
		void SetTexture(const char* acTexture);
		void Update();

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);

		void PackTo(NetPacket* pkNetPacket);
		void PackFrom(NetPacket* pkNetPacket);

};

ENGINE_API Property* Create_WaterRenderProperty();


#endif

