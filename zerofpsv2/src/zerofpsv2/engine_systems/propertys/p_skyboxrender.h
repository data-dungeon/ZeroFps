#ifndef _ENGINE_SKYBOXRENDERPROPERTY_H_
#define _ENGINE_SKYBOXRENDERPROPERTY_H_

#include "../../engine/property.h"
#include "../../basic/zfsystem.h"
#include <iostream>
#include "../../engine/zerofps.h"
#include "../engine_systems_x.h"
#include <string.h>
//#include "GL/glut.h"

using namespace std;

/// Property for a SkyBox.
class ENGINE_SYSTEMS_API P_SkyBoxRender : public Property {
		TextureManager* m_pkTexMan;
		ZSSZeroFps*		m_pkZeroFps;
		ZSSRender*	m_pkRender;	
	
		int m_iHor;		
		int m_iTop;
	
		Vector3 m_kRotateSpd;
		Vector3 m_kHead;
		
		int m_iTexture[SKYBOXSIDE_MAX];

		bool	m_bModeSixSides;

	public:
		P_SkyBoxRender(const char* acHor,const char* acTop);
		void SetTexture(const char* acHor,const char* acTop);			//the skybox textures acHor is the horisontal texture, actop is top and botom texture
		void SetRotate(Vector3 kRotate);
		void Update();

};

Property* Create_SkyBoxRenderProperty();

#endif

