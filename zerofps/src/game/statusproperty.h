#ifndef _STATUSPROPERTY_H_
#define _STATUSPROPERTY_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
//#include "playerobject.h"

using namespace std;

class StatusProperty: public Property {
	private:
	
		ObjectManager *m_pkObjectMan;
		ZeroFps *m_pkFps;
			
	public:
		float m_fArmor;
		float m_fArmorModifier;
		float m_fHealthModifier;		
		float m_fHealth;	
			
		StatusProperty();			
		
		void Damage(float fDamage);
		
//		void Update();		
//		void Touch(Object* pkObject);
};

Property* Create_StatusProperty();


#endif



