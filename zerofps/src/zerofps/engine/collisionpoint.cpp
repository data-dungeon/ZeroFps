#include "collisionpoint.h"

CollisionPoint::CollisionPoint(Vector3 kNewPos) {
	m_kPos=kNewPos;

}

bool CollisionPoint::Collide(CollisionObject *kOther,bool bContinue) {
	if(typeid(*kOther)==typeid(CollisionPoint)){
		CollisionPoint *kCp = dynamic_cast<CollisionPoint*>(kOther);			
		return CollidePoint(kCp);
		
	} else if(bContinue){
		cout<<"Unhandled Collision,Asking kOther"<<endl;	
		return kOther->Collide(this,false);
	
	}
		
	return false;
}


bool CollisionPoint::CollidePoint(CollisionPoint *kCp) {
	return (kCp->m_kPos==m_kPos);
}



