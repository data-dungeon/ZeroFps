#ifndef _COLLISIONHEIGHTMAP_H_
#define _COLLISIONHEIGHTMAP_H_


#include <typeinfo>
#include "../basic/basic.pkg"
#include "objects.pkg"
//#include "collisionobject.h"

class ENGINE_API CollisionHeightMap:public CollisionObject
{
	private:
		HeightMap* m_pkHM;
	
	public:
		CollisionHeightMap(HeightMap *pkHeightMap);		
		
		bool Collide(CollisionObject *kOther,bool bContinue);

		bool CollideSphere(CollisionSphere *pkCs);
		bool CollidePoint(CollisionPoint *pkCp);
};

#endif



