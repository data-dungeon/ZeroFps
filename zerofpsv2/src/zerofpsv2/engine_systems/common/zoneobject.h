#ifndef _ENGINE_ZONEOBJECT_H_
#define _ENGINE_ZONEOBJECT_H_
 
#include "../../engine/zerofps.h"
#include "../engine_systems_x.h"

class ENGINE_SYSTEMS_API ZoneObject: public Object {
	private:
		ZeroFps *m_pkFps;
		
	public:
		ZoneObject();
		void SetRadius(float fRadius);
};


#endif

