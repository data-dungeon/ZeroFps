#include "collisionpoint.h"

CollisionPoint::CollisionPoint(Vector3 *kNewPos) {
	m_kPos=kNewPos;
}
CollisionPoint::CollisionPoint(Vector3 kNewPos) {
	m_kPos=new Vector3();
	*m_kPos=kNewPos;
}

bool CollisionPoint::Collide(CollisionObject *kOther,Vector3 *pkPos,bool bContinue) {
	if(typeid(*kOther)==typeid(CollisionPoint)){
		CollisionPoint *kCp = dynamic_cast<CollisionPoint*>(kOther);			
		return CollidePoint(kCp,pkPos);
		
	} else if(bContinue){
//		cout<<"Unhandled Collision,Asking kOther"<<endl;	
		return kOther->Collide(this,pkPos,false);
	
	}
		
	return false;
}


bool CollisionPoint::CollidePoint(CollisionPoint *kCp,Vector3 *pkPos) {
	if((*kCp->m_kPos)==(*m_kPos)){
		*pkPos=*m_kPos;
		return true;
	}
	return false;
}



