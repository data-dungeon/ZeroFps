#include "collisionproperty.h"

CollisionProperty::CollisionProperty(Vector3 kPos,float iRadius) 
{
	m_pkColObject=new CollisionSphere(kPos,iRadius);
}

CollisionProperty::CollisionProperty(Vector3 kPos) 
{
	m_pkColObject=new CollisionPoint(kPos);
}

CollisionProperty::CollisionProperty(Vector3 *kPos,float *iRadius) 
{
	m_pkColObject=new CollisionSphere(kPos,iRadius);
}

CollisionProperty::CollisionProperty(Vector3 *kPos) 
{
	m_pkColObject=new CollisionPoint(kPos);
}

void CollisionProperty::Update() {


}




