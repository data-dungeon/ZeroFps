#ifndef _ENGINE_AUTOPARENTPROPERTY_H_
#define _ENGINE_AUTOPARENTPROPERTY_H_

#include "../basic/basic.pkg"
#include "../engine/engine.pkg"
#include "property.h"
#include <iostream>
#include <limits.h>

#include <string.h>

using namespace std;

class ENGINE_API AutoParentProperty: public Property {
	private:
		ZeroFps* m_pkFps;
		
		float m_fUpdateTime;
		float m_fLastUpdate;

	public:
		AutoParentProperty();
		void Update();		
};

Property* Create_AutoParentProperty();

#endif




