#ifndef _MACHINEGUNPROJECTILE_H_
#define _MACHINEGUNPROJECTILE_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
//#include "playerobject.h"
#include "common_x.h"

using namespace std;

class COMMON_API MachineGunProjectile: public Property {
	private:	
		ObjectManager *m_pkObjectMan;
		ZeroFps *m_pkFps;			
		float m_fCreateTime;		
		int m_iMode;
		
	public:
		int shoterid;
		
		MachineGunProjectile();	
		void CloneOf(Property* pkProperty) { }
		void Update();
		void Touch(Collision* pkCol);		
};



COMMON_API Property* Create_MachineGunProjectile();



#endif



