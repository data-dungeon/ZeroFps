#include "collisionproperty.h"

CollisionProperty::CollisionProperty(Vector3 kPos,float iRadius) 
{
	m_pkColObject=new CollisionSphere(kPos,iRadius);
	Init();
}

CollisionProperty::CollisionProperty(Vector3 kPos) 
{
	m_pkColObject=new CollisionPoint(kPos);
	Init();
}

CollisionProperty::CollisionProperty(Vector3 *kPos,float *iRadius) 
{
	m_pkColObject=new CollisionSphere(kPos,iRadius);
	Init();
}

CollisionProperty::CollisionProperty(Vector3 *kPos) 
{	
	m_pkColObject=new CollisionPoint(kPos);
	Init();
}

CollisionProperty::CollisionProperty(HeightMap *pkHeightMap) 
{	
	m_pkColObject=new CollisionHeightMap(pkHeightMap);
	Init();
}



CollisionProperty::~CollisionProperty() {
//	cout<<"tarbort collision property"<<endl;
	if(m_pkCollisionMan!=NULL){
//		cout<<"Tar bort sig från collisionmanager"<<endl;
		m_pkCollisionMan->Remove(this);		
	}
}


void CollisionProperty::Init(){
	strcpy(m_acName,"CollisionProperty");		
	m_pkCollisionMan=NULL;
}

void CollisionProperty::Update() {


}




