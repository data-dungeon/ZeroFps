#ifndef _MASSDRIVERPROPERTY_H_
#define _MASSDRIVERPROPERTY_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "massdriverprojectile.h"
//#include "playerobject.h"

using namespace std;

#define PROJECTILE_SPEED 200

class MassDriverProperty: public Property {

	private:
		Input *m_pkInput;
		ObjectManager *m_pkObjectMan;
		ZeroFps *m_pkFps;
				
				
		int m_iAmmo;
		Vector3 m_kAim;
	
		int m_iActionFire;
	
		float m_fLastShot;
	
	public:
		MassDriverProperty();
		
		void Update();
		
		void Fire();

};

Property* Create_MassDriverProperty();


#endif





