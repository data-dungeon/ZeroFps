#ifndef _BUNNYOBJECT_H_
#define _BUNNYOBJECT_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/basic/basic.pkg"

//#include "floatproperty.h"
//#include "gravityproperty.h"
#include "ballobject.h"
#include "bunnyaiproperty.h"

class BunnyObject: public Object {
	private:
		ZeroFps		*m_pkFps;

	public:
		bool onGround;
	
		BunnyObject();
		
		void HandleCollision(Object* pkObject,Vector3 kPos,bool bContinue);

};


#endif




