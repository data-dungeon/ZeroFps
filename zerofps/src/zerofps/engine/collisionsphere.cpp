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



bool CollisionSphere::Collide(CollisionObject *kOther,Vector3 *pkPos,bool bContinue) {
	if(typeid(*kOther)==typeid(CollisionSphere)){
		CollisionSphere *kCs=dynamic_cast<CollisionSphere*>(kOther);
		return CollideSphere(kCs,pkPos);

	} else if(typeid(*kOther)==typeid(CollisionPoint)){
		CollisionPoint *kCp = dynamic_cast<CollisionPoint*>(kOther);			
		return CollidePoint(kCp,pkPos);
		
	} else if(bContinue){
//		cout<<"Unhandled Collision,Asking kOther"<<endl;	
		return kOther->Collide(this,pkPos,false);
	
	}
		
	return false;
}

bool CollisionSphere::CollideSphere(CollisionSphere *kCs,Vector3 *pkPos) {
	Vector3 kDistance=((*kCs->m_kPos)-(*m_kPos));
	

	if( kDistance.Length() < (*m_fRadius + *kCs->m_fRadius) ){
		kDistance.Normalize();
		
//		kDistance.Print();
//		cout<<endl;
		
		*pkPos= *m_kPos + (kDistance * (*m_fRadius));
		return true;
	}
	
	return false;
}

bool CollisionSphere::CollidePoint(CollisionPoint *kCp,Vector3 *pkPos) {
//	float fDistance=((*m_kPos)-(*kCp->m_kPos)).Length();

	Vector3 kDistance=((*kCp->m_kPos)-(*m_kPos));
	
	if( kDistance.Length() < (*m_fRadius) ){
//		*pkPos= *m_kPos + kDistance;
		kDistance.Normalize();
		*pkPos= *m_kPos + (kDistance * (*m_fRadius));
		return true;
	}
	
	return false;
}




