#ifndef _TREEIDLEPROPERTY_H_
#define _TREEIDLEPROPERTY_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>

using namespace std;

class TreeIdleProperty: public Property {
	private:
		ZeroFps *m_pkFps;
		float m_fWind;
		float m_fRand;
		
		vector<PropertyValues> GetPropertyValues();
	public:
		TreeIdleProperty();
		void Update();
		
		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);
};

Property* Create_TreeIdleProperty();


#endif
