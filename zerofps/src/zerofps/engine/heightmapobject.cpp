#include "heightmapobject.h"

HeightMapObject::HeightMapObject(HeightMap *pkMap) {
	m_kName="HeightMapObject";
	m_pkHeightMap=pkMap;
	m_bSave=false;

	AddProperty(new CollisionProperty(pkMap));

/*
	AddProperty("HeightMapRenderProperty");
	(static_cast<HeightMapRenderProperty*>(GetProperty("HeightMapRenderProperty")))->SetHeightMap(pkMap);	
*/	
	
	AddProperty("HMRP2");
	(static_cast<HMRP2*>(GetProperty("HMRP2")))->SetHeightMap(pkMap);	


}

