#ifndef _SKYBOXOBJECT_H_
#define _SKYBOXOBJECT_H_
 
#include "objects.pkg"
//#include "../basic/heightmap.h"

class ENGINE_API SkyBoxObject: public Object {
	private:
//		HeightMap *m_pkHeightMap;

	public:
		SkyBoxObject(const char* acHor,const char* acTop);
		void SetTexture(const char* acHor,const char* acTop);
};


#endif

