#ifndef _MASSDRIVERPROJECTILE_H_
#define _MASSDRIVERPROJECTILE_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "common_x.h"
//#include "playerobject.h"

using namespace std;

class COMMON_API MassDriverProjectile: public Property {
	private:	
		ObjectManager *m_pkObjectMan;
		ZeroFps *m_pkFps;			
		float m_fCreateTime;		
		int m_iMode;
		
	public:
		int shoterid;
		
		MassDriverProjectile();	
		void CloneOf(Property* pkProperty) { }
		void Update();
		void Touch(Collision* pkCol);		
};



COMMON_API Property* Create_MassDriverProjectile();



#endif



