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
		CollisionShape* m_pkColSphere;
		CollisionShape* m_pkColObject;

	public:
		bool m_bGravity;
		bool m_bMove;
	
		PhysicProperty();

		CollisionData* Test(PhysicProperty* kOther);

		CollisionShape* GetColSphere(){return m_pkColSphere;};
		CollisionShape* GetColShape(){return m_pkColObject;};
};

Property* Create_PhysicProperty();


#endif







