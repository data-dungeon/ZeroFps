#include "collisionmanager.h"

void CollisionManager::Add(Object *kObject)
{

	Property *pkProperty=kObject->GetProperty("CollisionProperty");
	//Check if the Object has a collisionproperty else return	
	if(pkProperty==NULL)
		return;

	CollisionProperty *pkColProp=dynamic_cast<CollisionProperty*>(pkProperty);
	pkColProp->SetColMan(this);
	m_akColPropertys.push_back(pkColProp);
}
/*
void CollisionManager::Add(CollisionProperty* kColProperty)
{
	kColProperty->SetColMan(this);
	m_akColPropertys.push_back(kColProperty);
}
*/
void CollisionManager::Remove(Object *pkObject)
{
	CollisionProperty *pkColProp=dynamic_cast<CollisionProperty*>(pkObject->GetProperty("CollisionProperty"));
	pkColProp->SetColMan(NULL);
	m_akColPropertys.remove(pkColProp);
}

void CollisionManager::Remove(CollisionProperty* pkColProperty)
{
	m_akColPropertys.remove(pkColProperty);
}

void CollisionManager::Update() 
{
	for(list<CollisionProperty*>::iterator itx=m_akColPropertys.begin();itx!=m_akColPropertys.end();itx++) 
	{
		if((*itx)->GetObject()->GetStatic())
			continue;
		
		for(list<CollisionProperty*>::iterator ity=itx;ity!=m_akColPropertys.end();ity++) 
		{
			//jump past the first one
			if(itx==ity)
				continue;
			
			Vector3 kCollisionPoint(0,0,0);			
			
			if((*itx)->GetColObject()->Collide((*ity)->GetColObject(),&kCollisionPoint,true)){

				(*itx)->GetObject()->HandleCollision((*ity)->GetObject(),kCollisionPoint,true);
			}
		}
	}
}

