#include "collisionmanager.h"

void CollisionManager::Add(Object *kObject)
{
	CollisionProperty *pkColProp=dynamic_cast<CollisionProperty*>(kObject->GetProperty("CollisionProperty"));
	pkColProp->SetColMan(this);
	m_akColPropertys.push_back(pkColProp);
}

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
		for(list<CollisionProperty*>::iterator ity=itx;ity!=m_akColPropertys.end();ity++) 
		{
			//jump past the first one
			if(itx==ity)
				continue;
			
			if((*itx)->GetColObject()->Collide((*ity)->GetColObject(),true)){
				if((*ity)->GetObject()->GetProperty("PlayerControlProperty")==NULL)
					(*ity)->GetObject()->Remove();
			}
		}
	}
}

