#ifndef _PLAYEROBJECT_H_
#define _PLAYEROBJECT_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/basic/basic.pkg"
#include "playercontrolproperty.h"
#include "gravityproperty.h"
//#include "heightmapobject.h"

class PlayerObject: public Object {
	private:

	public:
		bool onGround;
	
		PlayerObject(HeightMap *pkMap,Input *pkInput,ZeroFps *pkFps);
		
		void HandleCollision(Object* pkObject,Vector3 kPos,bool bContinue);


};


#endif



