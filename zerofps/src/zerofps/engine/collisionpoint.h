#ifndef _COLLISIONPOINT_H_
#define _COLLISIONPOINT_H_


#include <typeinfo>
#include "../basic/basic.pkg"
#include "collisionobject.h"

class ENGINE_API CollisionPoint:public CollisionObject{
	private:
		
	public:
		Vector3 m_kPos;
	
		CollisionPoint(Vector3 kNewPos);
		
		bool Collide(CollisionObject *kOther,bool bContinue);

};

#endif

