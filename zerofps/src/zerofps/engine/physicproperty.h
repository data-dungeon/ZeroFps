#ifndef _PHYSICPROPERTY_H_
#define _PHYSICPROPERTY_H_

#include "property.h"
#include "collisionshape.h"
#include <vector>

using namespace std;

class CollisionShape;

class ENGINE_API PhysicProperty :public Property
{
	private:
		CollisionShape* m_kColSphere;
		CollisionShape* m_kColObject;

	public:
		bool m_bGravity;
		bool m_bMove;
	
		PhysicProperty();

		CollisionData* Test(PhysicProperty* kOther);

};

Property* Create_PhysicProperty();


#endif







