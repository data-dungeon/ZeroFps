#ifndef _MACHINEGUNPROPERTY_H_
#define _MACHINEGUNPROPERTY_H_

#include "inventoryproperty.h"

/*
#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "massdriverprojectile.h"
//#include "playerobject.h"
*/

using namespace std;

#define PROJECTILE_SPEED 150

class MachineGunProperty: public InventoryProperty {

	private:
		Input *m_pkInput;
		ObjectManager *m_pkObjectMan;
		ZeroFps *m_pkFps;
		OpenAlSystem* m_pkAlSys;		
				
		int m_iAmmo;
		Vector3 m_kAim;
	
		int m_iActionFire;
	
		float m_fLastShot;
		float m_fFireRate;
	
		Sound *firesound;					
	
		bool m_bAdded;
		bool m_bFiring;
	
	public:
		MachineGunProperty();
		~MachineGunProperty();
		
		
		void Update();
		
		void Use();

};

Property* Create_MachineGunProperty();


#endif





