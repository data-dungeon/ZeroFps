#ifndef _ENGINE_HEIGHTMAP2_H_
#define _ENGINE_HEIGHTMAP2_H_

#include "../../engine/property.h"
#include <iostream>
#include "../common/heightmap2.h"
#include "../../engine/zerofps.h"
#include "../engine_systems_x.h"

using namespace std;

/// Property to add a Heightmap to an object.
class ENGINE_SYSTEMS_API P_Heightmap2: public Property {
		Heightmap2	m_kMap;
	
		Render* m_pkRender;		
		ZeroFps* m_pkZeroFps;		
		
	public:
		P_Heightmap2();
		void Update();

//		void PackTo(NetPacket* pkNetPacket);
//		void PackFrom(NetPacket* pkNetPacket);
};

Property* Create_P_Heightmap2();

#endif

