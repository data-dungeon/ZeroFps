#ifndef _TELEPORTPROPERTY_H_
#define _TELEPORTPROPERTY_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/basic/basic.pkg"
#include "../zerofps/engine/openalsystem.h"
#include <iostream>
#include "statusproperty.h"
#include "common_x.h"

using namespace std;

class COMMON_API TeleportProperty: public Property {
	private:
		Vector3 m_kToPos;
		OpenAlSystem* m_pkAlSys;
		ObjectManager *m_pkObjectMan;
		StatusProperty* m_pkStatusProperty;
		
	
		Sound *sound;
		Sound *sound2;		
		vector<PropertyValues> GetPropertyValues();

	public:
		TeleportProperty();
		~TeleportProperty();
		void CloneOf(Property* pkProperty) { }
		
		void Update();
		void Touch(Collision* pkCol);		
	
		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);
	
		void SetToPos(Vector3 kPos){m_kToPos=kPos;};
	
	
};

COMMON_API Property* Create_TeleportProperty();

#endif





