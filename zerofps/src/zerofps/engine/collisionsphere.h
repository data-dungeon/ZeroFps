#ifndef _COLLISIONSPHERE_H_
#define _COLLISIONSPHERE_H_


#include <typeinfo>
#include "../basic/basic.pkg"
//#include "objects.pkg"
//#include "collisionobject.h"
#include "objects.pkg"

class ENGINE_API CollisionSphere:public CollisionObject{
	private:
		
	public:
		Vector3 m_kPos;
		float m_fRadius;	
	
		CollisionSphere();
		
		bool Collide(CollisionObject *kOther,bool bContinue);

};

#endif

