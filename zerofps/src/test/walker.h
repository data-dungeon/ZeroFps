#ifndef _WALKEROBJECT_H_
#define _WALKEROBJECT_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/basic/basic.pkg"
#include "normaldrawproperty.h"
#include "playerballobject.h"

class WalkerObject: public Object {
	private:
		ZeroFps		*m_pkFps;

	public:
		bool onGround;	

		WalkerObject();
		~WalkerObject();
		
		void HandleCollision(Object* pkObject,Vector3 kPos,bool bContinue);
	
};


#endif

