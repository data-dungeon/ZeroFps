#ifndef _ENGINE_SKYBOXRENDERPROPERTY_H_
#define _ENGINE_SKYBOXRENDERPROPERTY_H_

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
	
		Vector3 m_kRotateSpd;
		Vector3 m_kHead;
		
		int m_iTexture[SKYBOXSIDE_MAX];

		bool	m_bModeSixSides;

	public:
		SkyBoxRenderProperty(const char* acHor,const char* acTop);
		void CloneOf(Property* pkProperty) { }
		void SetTexture(const char* acHor,const char* acTop);			//the skybox textures acHor is the horisontal texture, actop is top and botom texture
		void SetRotate(Vector3 kRotate);
		void Update();

};

Property* Create_SkyBoxRenderProperty();

#endif

