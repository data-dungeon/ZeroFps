#ifndef _COLLISIONSPHERE_H_
#define _COLLISIONSPHERE_H_


#include <typeinfo>
#include "../basic/basic.pkg"
#include "objects.pkg"

class ENGINE_API CollisionSphere:public CollisionObject{
	private:
		
	public:
		Vector3 *m_kPos;
		float *m_fRadius;
	
		CollisionSphere(Vector3 kPos,float fRadius);
		CollisionSphere(Vector3 *kPos,float *fRadius);
		
		bool Collide(CollisionObject *kOther,Vector3 *pkPos,bool bContinue);
		
		bool CollideSphere(CollisionSphere *kCs,Vector3 *pkPos);
		bool CollidePoint(CollisionPoint *kCp,Vector3 *pkPos);

		float GetBoundingRadius() {return *m_fRadius;};

};

#endif

