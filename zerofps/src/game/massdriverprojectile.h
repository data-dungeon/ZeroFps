#ifndef _MASSDRIVERPROJECTILE_H_
#define _MASSDRIVERPROJECTILE_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
//#include "playerobject.h"

using namespace std;

class MassDriverProjectile: public Property {
	private:
	
		ObjectManager *m_pkObjectMan;
		ZeroFps *m_pkFps;
			
		float m_fCreateTime;
			
	public:
		MassDriverProjectile();
	
		void Update();
		void Touch(Object* pkObject);

};



Property* Create_MassDriverProjectile();



#endif



