#ifndef _PLAYEROBJECT_H_
#define _PLAYEROBJECT_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/basic/basic.pkg"
#include "playercontrolproperty.h"
#include "gravityproperty.h"
#include "ballobject.h"
//#include "heightmapobject.h"

class PlayerObject: public Object {
	private:
		ZeroFps *m_pkFps;

	public:
		bool onGround;
	
		PlayerObject(HeightMap *pkMap,Input *pkInput,ZeroFps *pkFps);
		
		void HandleCollision(Object* pkObject,Vector3 kPos,bool bContinue);

		void ObjectUpdate();
};


#endif



