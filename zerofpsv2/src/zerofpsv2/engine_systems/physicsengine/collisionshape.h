#ifndef _ENGINE_COLLISIONSHAPE_H_
#define _ENGINE_COLLISIONSHAPE_H_

#include <typeinfo>
#include "physicsengine.h"
#include "../engine_systems_x.h"
#include "../propertys/physicproperty.h"

using namespace std;


struct Collision;

class ENGINE_SYSTEMS_API CollisionShape
{
	protected:
		
	public:
		P_Physic* m_pkPP;			
		PhysicsEngine* m_pkPhysEngine;
		
		void SetPPPointer(P_Physic* pkPP);
		
		virtual Collision* Test(CollisionShape* kOther,bool bContinue)=0;
};

#endif
















