#ifndef _SKYBOXRENDERPROPERTY_H_
#define _SKYBOXRENDERPROPERTY_H_

#include "../basic/basic.pkg"
#include "../render/render.pkg"
#include "../engine/engine.pkg"
#include "property.h"
#include <iostream>

#include <string.h>
//#include "GL/glut.h"

using namespace std;


class ENGINE_API SkyBoxRenderProperty: public Property {
		TextureManager* m_pkTexMan;
		ZeroFps* m_pkZeroFps;
		Render* m_pkRender;	
	
		int m_iHor;
		int m_iTop;
	
	public:
		SkyBoxRenderProperty(const char* acHor,const char* acTop);
		void SetTexture(const char* acHor,const char* acTop);
		void Update();

};


#endif

