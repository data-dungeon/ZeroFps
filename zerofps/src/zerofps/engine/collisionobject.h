#ifndef _COLOSIONOBJECT_H_
#define _COLOSIONOBJECT_H_

#include "engine_x.h"

class ENGINE_API CollisionObject {
	protected:


	public:
	
		// kOther  : the collisionobject to check collision against	
		// pkPos   : pointer where the collision funktion stores the exact collision point
		// bContinue:  if true and the object could not handle the collision sen collision test to kOther
		virtual bool Collide(CollisionObject *kOther,Vector3 *pkPos,bool bContinue)=0;
};

#endif









