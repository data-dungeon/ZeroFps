#ifndef _PLAYERBALLOBJECT_H_
#define _PLAYERBALLOBJECT_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/basic/basic.pkg"
#include "normaldrawproperty.h"
#include "playercontrolproperty.h"
//#include "heightmapobject.h"

class PlayerBallObject: public Object {
	private:


	public:
		PlayerBallObject(HeightMap *pkMap,Input *pkInput,ZeroFps *pkFps);
		
		void HandleCollision(Object* pkObject,Vector3 kPos,bool bContinue);


};


#endif












