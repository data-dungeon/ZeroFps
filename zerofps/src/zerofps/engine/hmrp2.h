#ifndef _ENGINE_HMRP2_H_
#define _ENGINE_HMRP2_H_

#include "../basic/basic.pkg"
#include "../render/render.pkg"
#include "property.h"
#include <iostream>
#include "../engine/engine.pkg"
#include "heightmap.h"

#include "GL/glut.h"

using namespace std;


class ENGINE_API HMRP2: public Property {
		HeightMap* m_pkHeightMap;
		Frustum* m_pkFrustum;
		TextureManager* m_pkTexMan;
		ZeroFps* m_pkZeroFps;
		Render* m_pkRender;	
	
	
	public:
		HMRP2(HeightMap* pkHeightMap);
		void CloneOf(Property* pkProperty) { }
		void Update();

		void SetHeightMap(HeightMap* pkHeightMap);
		HeightMap* GetHeightMap() {return m_pkHeightMap;};
};

Property* Create_HMRP2();

#endif

