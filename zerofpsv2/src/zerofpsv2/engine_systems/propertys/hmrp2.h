#ifndef _ENGINE_HMRP2_H_
#define _ENGINE_HMRP2_H_

#include "../../engine/property.h"
#include <iostream>
#include "../common/heightmap.h"
#include "../engine_systems_x.h"
#include "GL/glut.h"

using namespace std;

/// Property to add a Hmap to a object.
class ENGINE_SYSTEMS_API HMRP2: public Property {
		HeightMap* m_pkHeightMap;
		TextureManager* m_pkTexMan;
		ZeroFps* m_pkZeroFps;
		Render* m_pkRender;	
	
	
	public:
		string	m_strMapName;

		void SetPolyMode(int iMode);
		HMRP2(HeightMap* pkHeightMap, string strMapName);
		void CloneOf(Property* pkProperty) { }
		void Update();

		void SetHeightMap(HeightMap* pkHeightMap, string strMapName);
		HeightMap* GetHeightMap() {return m_pkHeightMap;};
		void SetPolyMode(PolygonMode eMode);



		void PackTo(NetPacket* pkNetPacket, int iConnectionID);
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID);
};

Property* Create_HMRP2();

#endif

