#ifndef _TREEIDLEPROPERTY_H_
#define _TREEIDLEPROPERTY_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "common_x.h"

using namespace std;

class COMMON_API TreeIdleProperty: public Property {
	private:
		ZeroFps *m_pkFps;
		float m_fWind;
		float m_fRand;
		
		vector<PropertyValues> GetPropertyValues();
	public:
		TreeIdleProperty();
		void CloneOf(Property* pkProperty) { }
		void Update();
		
		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);
};

COMMON_API Property* Create_TreeIdleProperty();


#endif
