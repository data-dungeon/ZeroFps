#ifndef _ENGINE_COLLISIONSHAPE_H_
#define _ENGINE_COLLISIONSHAPE_H_

#include <typeinfo>
#include "physicsengine.h"
#include "engine_x.h"

using namespace std;


struct Collision;

class ENGINE_API CollisionShape
{
	protected:
		
	public:
		PhysicProperty* m_pkPP;			
		PhysicsEngine* m_pkPhysEngine;
		
		void SetPPPointer(PhysicProperty* pkPP);
		
		virtual Collision* Test(CollisionShape* kOther,bool bContinue)=0;
};

#endif
















