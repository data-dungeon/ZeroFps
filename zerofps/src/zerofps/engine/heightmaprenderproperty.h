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
	
		int m_iAutoLod;
		int m_iDetail;
		int m_iViewDistance;
		int m_iFpsLock;
		int m_iSlicesize;
		int m_iLodUpdate;
	
	public:
		HeightMapRenderProperty(HeightMap* pkHeightMap);
		void Update();
		void DrawHMlod(HeightMap* kmap,Vector3 CamPos,int iFps);
		void GiveTexCor(float &iX,float &iY,int iNr);
};


#endif

