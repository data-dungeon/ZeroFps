#ifndef _HEIGHTMAPOBJECT_H_
#define _HEIGHTMAPOBJECT_H_

#include "objects.pkg"
#include "../basic/heightmap.h"

class ENGINE_API HeightMapObject: public Object {
	private:
		HeightMap *m_pkHeightMap;

	public:
		HeightMapObject(HeightMap *pkMap);
		HeightMap *GetHeightMap(){return m_pkHeightMap;};
};


#endif









