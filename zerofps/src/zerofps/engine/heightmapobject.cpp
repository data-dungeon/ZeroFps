#include "heightmapobject.h"

HeightMapObject::HeightMapObject(HeightMap *pkMap) {
	m_kName="HeightMapObject";
	m_pkHeightMap=pkMap;
	m_bSave=false;

	AddProperty(new CollisionProperty(pkMap));

	AddProperty("HeightMapRenderProperty");
	(static_cast<HeightMapRenderProperty*>(GetProperty("HeightMapRenderProperty")))->SetHeightMap(pkMap);	
	
//	AddProperty(new HeightMapRenderPropertyMK2(pkMap));
}

