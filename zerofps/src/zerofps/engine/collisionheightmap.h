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
		
		bool Collide(CollisionObject *kOther,Vector3 *pkPos,bool bContinue);

		bool CollideSphere(CollisionSphere *pkCs,Vector3 *pkPos);
		bool CollidePoint(CollisionPoint *pkCp,Vector3 *pkPos);
		
		float GetBoundingRadius() {return sqrt(m_pkHM->m_iHmSize*m_pkHM->m_iHmSize + m_pkHM->m_iHmSize*m_pkHM->m_iHmSize)/2 ;};
};

#endif



