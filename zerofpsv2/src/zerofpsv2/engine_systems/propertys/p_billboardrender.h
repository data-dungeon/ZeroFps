#ifndef _ENGINE_BILLBOARDRENDERPROPERTY_H_
#define _ENGINE_BILLBOARDRENDERPROPERTY_H_

#include "../../engine/property.h"
#include "../../basic/zfsystem.h"
#include <iostream>
#include <string>
#include "../engine_systems_x.h"

using namespace std;


class ENGINE_SYSTEMS_API P_BillBoardRender : public Property 
{
 		ZShaderSystem*		m_pkZShaderSystem;
		ZSSRender*			m_pkRender;	
			
		ZFResourceHandle* m_pkMaterial;
		string				m_strMaterial;
		float m_fScale;
		
		
		bool HandleSetValue( const string& kValueName ,const string& kValue );		
		void DrawBillboard();
	
	public:
		P_BillBoardRender();
		~P_BillBoardRender();
		
		void Update();

		void SetMaterial(const string& strMaterial);
		void SetScale(float fScale) 							{m_fScale=fScale;	SetNetUpdateFlag(true);	};
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		
};

Property* Create_BillBoardRenderProperty();

#endif

