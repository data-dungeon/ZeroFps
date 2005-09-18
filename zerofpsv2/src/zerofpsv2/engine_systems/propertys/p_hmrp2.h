#ifndef _ENGINE_HMRP2_H_
#define _ENGINE_HMRP2_H_
 
#include "../../engine/property.h"
#include <iostream>
#include "../engine_systems_x.h"
#include "../../render/light.h"

using namespace std;

class HeightMap;

/// Property to add a Hmap to a object.
class ENGINE_SYSTEMS_API P_HMRP2: public Property 
{
	private:
		TextureManager*	m_pkTexMan;
		ZeroFps*				m_pkZeroFps;
		ZSSRender*			m_pkRender;	
		ZSSLight*			m_pkLight;	
		
		LightProfile		m_kLightProfile;				//used for lighting
		
		vector<PropertyValues> GetPropertyValues();

	public:
		HeightMap*			m_pkHeightMap;
		string				m_strMapName;		// Name of file to load/save to.

		P_HMRP2();
		~P_HMRP2();

		float GetRadius();
		void Update();

		void SetHeightMap(HeightMap* pkHeightMap, string strMapName);
		HeightMap* GetHeightMap() {return m_pkHeightMap;};

		void Init();

		void PackTo(NetPacket* pkNetPacket, int iConnectionID);
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID);
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
};

Property* Create_HMRP2();
void ENGINE_SYSTEMS_API Register_PHmrp(ZeroFps* pkZeroFps);


#endif

