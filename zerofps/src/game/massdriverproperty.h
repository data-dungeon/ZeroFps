#ifndef _MASSDRIVERPROPERTY_H_
#define _MASSDRIVERPROPERTY_H_

#include "inventoryproperty.h"
#include "massdriverprojectile.h"

using namespace std;

#define PROJECTILE_SPEED 150

class MassDriverProperty: public InventoryProperty {

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
		MassDriverProperty();
		~MassDriverProperty();
		
		
		void Update();
		
		void Use();

};

Property* Create_MassDriverProperty();


#endif





