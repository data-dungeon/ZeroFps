#ifndef _COLLISIONSHAPE_H_
#define _COLLISIONSHAPE_H_

using namespace std;

#include <typeinfo>
//#include "physicproperty.h"
#include "physicsengine.h"
#include "engine_x.h"

class ENGINE_API CollisionShape
{
	protected:
		
	public:
		PhysicProperty* m_pkPP;			
		Vector3 *m_pkPos;		
		
		void SetPPPointer(PhysicProperty* pkPP);
		
		virtual CollisionData* Test(CollisionShape* kOther,bool bContinue)=0;
};

#endif
















