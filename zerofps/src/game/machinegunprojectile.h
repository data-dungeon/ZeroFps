#ifndef _MACHINEGUNPROJECTILE_H_
#define _MACHINEGUNPROJECTILE_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
//#include "playerobject.h"

using namespace std;

class MachineGunProjectile: public Property {
	private:	
		ObjectManager *m_pkObjectMan;
		ZeroFps *m_pkFps;			
		float m_fCreateTime;		
		int m_iMode;
		
	public:
		int shoterid;
		
		MachineGunProjectile();	
		void Update();
		void Touch(Collision* pkCol);		
};



Property* Create_MachineGunProjectile();



#endif



