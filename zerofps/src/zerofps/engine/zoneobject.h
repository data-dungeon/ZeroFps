#ifndef _ENGINE_ZONEOBJECT_H_
#define _ENGINE_ZONEOBJECT_H_
 
#include "objects.pkg"
#include "zerofps.h"

class ENGINE_API ZoneObject: public Object {
	private:
		ZeroFps *m_pkFps;
		
	public:
		ZoneObject();
		void SetRadius(float fRadius);
};


#endif

