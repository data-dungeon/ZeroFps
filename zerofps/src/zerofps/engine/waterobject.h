#ifndef _ENGINE_WATEROBJECT_H_
#define _ENGINE_WATEROBJECT_H_
 
#include "objects.pkg"
//#include "../basic/heightmap.h"

class ENGINE_API WaterObject: public Object {
	private:
//		HeightMap *m_pkHeightMap;

	public:
		WaterObject(int iSize,int iStep,const char* acTexture);
		void SetTexture(const char* acTexture);
};


#endif



