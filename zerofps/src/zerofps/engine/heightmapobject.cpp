#include "heightmapobject.h"

HeightMapObject::HeightMapObject(HeightMap *pkMap) {
	m_pkHeightMap=pkMap;
	AddProperty(new CollisionProperty(pkMap));
}

