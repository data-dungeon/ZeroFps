#ifndef _TELEPORTPROPERTY_H_
#define _TELEPORTPROPERTY_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/basic/basic.pkg"
#include "../zerofps/engine/openalsystem.h"
#include <iostream>

using namespace std;

class TeleportProperty: public Property {
	private:
		Vector3 m_kToPos;
		OpenAlSystem* m_pkAlSys;
	
		Sound *sound;
		Sound *sound2;		
	
	public:
		TeleportProperty();
	
		void Update();
		void Touch(Object* pkObject);		
	
		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);
	
		void SetToPos(Vector3 kPos){m_kToPos=kPos;};
	
};

Property* Create_TeleportProperty();

#endif





