#ifndef _ENGINE_HMRP2_H_
#define _ENGINE_HMRP2_H_

#include "../../engine/property.h"
#include <iostream>
#include "../common/heightmap.h"
#include "../engine_systems_x.h"
#include "../../render/light.h"
//#include "GL/glut.h"

using namespace std;

/// Property to add a Hmap to a object.
class ENGINE_SYSTEMS_API P_HMRP2: public Property 
{
		TextureManager* m_pkTexMan;
		ZeroFps* m_pkZeroFps;
		Render* m_pkRender;	
		Light* m_pkLight;	
	
	
	public:
		float GetRadius();

		HeightMap* m_pkHeightMap;

		string	m_strMapName;

		void SetPolyMode(int iMode);
		P_HMRP2(HeightMap* pkHeightMap, string strMapName);
		void Update();

		void SetHeightMap(HeightMap* pkHeightMap, string strMapName);
		HeightMap* GetHeightMap() {return m_pkHeightMap;};
		void SetPolyMode(PolygonMode eMode);

		void Init();


		void PackTo(NetPacket* pkNetPacket, int iConnectionID);
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID);

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);

};

Property* Create_HMRP2();

#endif

