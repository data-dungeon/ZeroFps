#include "collisionsphere.h"

CollisionSphere::CollisionSphere(Vector3 *kPos,float *fRadius) {
	m_kPos=kPos;	
	m_fRadius=fRadius;
}
CollisionSphere::CollisionSphere(Vector3 kPos,float fRadius) {
	m_kPos=new Vector3();
	*m_kPos=kPos;	
	m_fRadius=new float;
	*m_fRadius=fRadius;
}



bool CollisionSphere::Collide(CollisionObject *kOther,bool bContinue) {
	if(typeid(*kOther)==typeid(CollisionSphere)){
		CollisionSphere *kCs=dynamic_cast<CollisionSphere*>(kOther);
		return CollideSphere(kCs);

	} else if(typeid(*kOther)==typeid(CollisionPoint)){
		CollisionPoint *kCp = dynamic_cast<CollisionPoint*>(kOther);			
		return CollidePoint(kCp);
		
	} else if(bContinue){
		cout<<"Unhandled Collision,Asking kOther"<<endl;	
		return kOther->Collide(this,false);
	
	}
		
	return false;
}

bool CollisionSphere::CollideSphere(CollisionSphere *kCs) {
	float fDistance=((*m_kPos)-(*kCs->m_kPos)).Length();
	
	return fDistance<(*m_fRadius+(*kCs->m_fRadius));
}

bool CollisionSphere::CollidePoint(CollisionPoint *kCp) {
	float fDistance=((*m_kPos)-(*kCp->m_kPos)).Length();
	return fDistance<(*m_fRadius);
}




