#ifndef _ENGINE_AUTOPARENTPROPERTY_H_
#define _ENGINE_AUTOPARENTPROPERTY_H_

#include "../../engine/property.h"
#include <iostream>
#include <limits.h>
#include "../engine_systems_x.h"
#include <string.h>

using namespace std;

class ENGINE_SYSTEMS_API AutoParentProperty: public Property {
	private:
		ZeroFps* m_pkFps;
		
		float m_fUpdateTime;
		float m_fLastUpdate;

	public:
		void CloneOf(Property* pkProperty) { }
		AutoParentProperty();
		void Update();		
};

Property* Create_AutoParentProperty();

#endif




