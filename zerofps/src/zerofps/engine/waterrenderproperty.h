#ifndef _WATERRENDERPROPERTY_H_
#define _WATERRENDERPROPERTY_H_

#include "../basic/basic.pkg"
#include "../render/render.pkg"
#include "../engine/engine.pkg"
#include "property.h"
#include <iostream>

#include <string.h>
//#include "GL/glut.h"

using namespace std;


class ENGINE_API WaterRenderProperty: public Property {
		TextureManager* m_pkTexMan;
		ZeroFps* m_pkZeroFps;
		Render* m_pkRender;	
	
		int m_iTexture;		
	
//		Vector3 m_kPos;
//		Vector3 m_kHead;
		int m_iSize;
		int m_iStep;
		
	public:
		WaterRenderProperty(int iSize,int iStep,const char* acTexture);

		void SetProperty(int iSize,int iStep,const char* acTexture);
		void SetTexture(const char* acTexture);
		void Update();

};


#endif

