#ifndef _MACHINEGUNPROPERTY_H_
#define _MACHINEGUNPROPERTY_H_

#include "inventoryproperty.h"
#include "machinegunprojectile.h"
#include "common_x.h"

using namespace std;

class COMMON_API MachineGunProperty: public InventoryProperty {

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
		float m_fBulletVel;
	
		Sound *firesound;					
	
		bool m_bAdded;
		bool m_bFiring;
	
	public:
		MachineGunProperty();
		~MachineGunProperty();
		void CloneOf(Property* pkProperty) { }
	
		
		void Update();
		
		void Use();

};

COMMON_API Property* Create_MachineGunProperty();


#endif





