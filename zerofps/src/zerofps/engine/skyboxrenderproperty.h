#ifndef _SKYBOXRENDERPROPERTY_H_
#define _SKYBOXRENDERPROPERTY_H_

#include "../basic/basic.pkg"
#include "../render/render.pkg"
#include "../engine/engine.pkg"
#include "property.h"
#include <iostream>


//#include "GL/glut.h"

using namespace std;


class ENGINE_API SkyBoxRenderProperty: public Property {
		TextureManager* m_pkTexMan;
		ZeroFps* m_pkZeroFps;
		Render* m_pkRender;	
	
	
	public:
		SkyBoxRenderProperty(HeightMap* pkHeightMap);
		void Update();

};


#endif

