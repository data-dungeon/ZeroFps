#ifndef _ENGINE_WATERRENDERPROPERTY_H_
#define _ENGINE_WATERRENDERPROPERTY_H_

#include "../engine_systems_x.h"
#include "../../engine/property.h"
#include <iostream>
#include "../../basic/zfsystem.h"
#include <string.h>
#include "../../engine/zerofps.h"
#include "../../engine/entitymanager.h"


using namespace std;

/// Property to render a watersurface.
class ENGINE_SYSTEMS_API P_WaterRender : public Property 
{
		TextureManager* 	m_pkTexMan;
		ZSSZeroFps* 		m_pkZeroFps;
		ZSSRender*			m_pkRender;	
		ZShaderSystem*		m_pkZShaderSystem;
		ZSSLight*			m_pkLight;
		
		ZFResourceHandle* m_pkMaterial; 
		string				m_strMaterial;	
		LightProfile		m_kLightProfile;
		
		RenderPackage		m_kRenderPackage;
		RenderPackage		m_kDebugRenderPackage;
		
		int		m_iTexture;		
		string	m_sTexture;
		Vector3	m_kSize;
		int		m_iStep;
		float		m_fWave;

		bool		m_bHaveMesh;
		
		bool HandleSetValue( const string& kValueName ,const string& kValue );		
		void HaveSetValue( const string& kValueName);		
		
		void DrawSurface();
		void GenerateMesh();
	public:
		P_WaterRender();
		~P_WaterRender();
		void Init();

		void SetMaterial(const string& strMaterial);
		
		void GetRenderPackages(vector<RenderPackage*>&	kRenderPackages,const RenderState&	kRenderState);
		void Update();

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);

		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );

};

ENGINE_API Property* Create_WaterRenderProperty();


#endif

