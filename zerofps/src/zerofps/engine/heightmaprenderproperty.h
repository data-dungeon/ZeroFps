#ifndef _HEIGHTMAPRENDERPROPERTY_H_
#define _HEIGHTMAPRENDERPROPERTY_H_

#include "../basic/basic.pkg"
#include "../render/render.pkg"
#include "property.h"
#include <iostream>
#include "../engine/engine.pkg"

#include "GL/glut.h"

using namespace std;


class ENGINE_API HeightMapRenderProperty: public Property {
		HeightMap* m_pkHeightMap;
		Frustum* m_pkFrustum;
		TextureManager* m_pkTexMan;
		ZeroFps* m_pkZeroFps;
		Render* m_pkRender;	
	
	public:
		HeightMapRenderProperty(HeightMap* pkHeightMap);
		void Update();

		void SetHeightMap(HeightMap* pkHeightMap);
		HeightMap* GetHeightMap() {return m_pkHeightMap;};

};

Property* Create_HeightMapRenderProperty();

#endif

