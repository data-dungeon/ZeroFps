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
		bool m_bFloat;
	
		PhysicProperty();
		~PhysicProperty();		
		
		void Update();
		
		float GetBoundingRadius();
		
		CollisionShape* GetColSphere(){return m_pkColSphere;};
		CollisionShape* GetColShape(){return m_pkColObject;};

		void SetColShape(CollisionShape* pkCs){m_pkColObject = pkCs;};


		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);
};

Property* Create_PhysicProperty();


#endif







