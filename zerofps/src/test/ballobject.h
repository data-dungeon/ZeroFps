#ifndef _BALLOBJECT_H_
#define _BALLOBJECT_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/basic/basic.pkg"
#include "normaldrawproperty.h"
#include "playerballobject.h"


class BallObject: public Object {
	private:


	public:
		BallObject(HeightMap *pkMap);
		
		void HandleCollision(Object* pkObject,bool bContinue);


};


#endif












