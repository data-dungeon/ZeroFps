#ifndef _COLLISIONSHAPE_H_
#define _COLLISIONSHAPE_H_

#include <typeinfo>
//#include "physicproperty.h"
#include "physicsengine.h"
#include "engine_x.h"

using namespace std;

class Sphere;

class ENGINE_API CollisionShape
{
	protected:
		
	public:
		PhysicProperty* m_pkPP;			
		PhysicsEngine* m_pkPhysEngine;
//		Vector3 *m_pkPos;		
		
		void SetPPPointer(PhysicProperty* pkPP);
		
		virtual CollisionData* Test(CollisionShape* kOther,Sphere* pkSThis,Sphere* pkSOther,float fTime,bool bContinue)=0;
};

#endif
















