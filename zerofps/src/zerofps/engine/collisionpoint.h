#ifndef _ENGINE_COLLISIONPOINT_H_
#define _ENGINE_COLLISIONPOINT_H_


#include <typeinfo>
#include "../basic/basic.pkg"
#include "collisionobject.h"

class ENGINE_API CollisionPoint:public CollisionObject{
	private:
		
	public:
		Vector3 *m_kPos;
	
		CollisionPoint(Vector3 kNewPos);		
		CollisionPoint(Vector3 *kNewPos);				
		bool Collide(CollisionObject *kOther,Vector3 *pkPos,bool bContinue);		
		bool CollidePoint(CollisionPoint *kCp,Vector3 *pkPos);

		float GetBoundingRadius() {return 0;};
};

#endif

