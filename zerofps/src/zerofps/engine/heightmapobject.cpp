#include "heightmapobject.h"
#include "heightmapcs.h"

HeightMapObject::HeightMapObject(HeightMap *pkMap) {
	m_kName="HeightMapObject";
	m_pkHeightMap=pkMap;
	m_bSave=false;

	m_iObjectType=OBJECT_TYPE_STATIC;


	AddProperty("PhysicProperty");
	PhysicProperty* PP =static_cast<PhysicProperty*>(GetProperty("PhysicProperty"));
	PP->SetColShape(new HeightMapCS(pkMap));	
	PP->m_bFloat=false;		
	PP->m_bGravity=false;		

/*
	AddProperty("HeightMapRenderProperty");
	(static_cast<HeightMapRenderProperty*>(GetProperty("HeightMapRenderProperty")))->SetHeightMap(pkMap);	
*/	
	
	AddProperty("HMRP2");
	(static_cast<HMRP2*>(GetProperty("HMRP2")))->SetHeightMap(pkMap);	


}

