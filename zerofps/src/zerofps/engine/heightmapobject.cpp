#include "heightmapobject.h"

HeightMapObject::HeightMapObject(HeightMap *pkMap) {
	m_pkHeightMap=pkMap;
	m_bStatic=true;
	AddProperty(new CollisionProperty(pkMap));
	
}

