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
			
			if((*itx)->GetColObject()->Collide((*ity)->GetColObject(),true)){
				(*itx)->GetObject()->HandleCollision((*ity)->GetObject(),true);
			
			
/*				if((*ity)->GetObject()->GetProperty("PlayerControlProperty")==NULL){
					Object *test=new Object();
					test->AddProperty(new ModelProperty());
					test->AddProperty(new CollisionProperty(&test->GetPos(),new float(1)));		
					float x=(rand()%100)/100.0-0.5;
					float y=(rand()%100)/100.0-0.5;
					test->GetPos()=(*ity)->GetObject()->GetPos()+Vector3(x,0,y);
					test->GetStatic()=true;
					if((*ity)->GetObject()->GetObjectMan()!=NULL){
						(*ity)->GetObject()->GetObjectMan()->Add(test);
						Add(test);
					}
					(*ity)->GetObject()->Remove();
				}
*/
			}
		}
	}
}

