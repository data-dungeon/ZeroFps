#include "collisionpoint.h"

CollisionPoint::CollisionPoint(Vector3 kNewPos) {
	m_kPos=kNewPos;

}

bool CollisionPoint::Collide(CollisionObject *kOther,bool bContinue) {
//	cout<<typeid(*kOther).name()<<endl;
//	cout<<typeid(CollisionSphere).name()<<endl;	
	if(typeid(*kOther)==typeid(CollisionPoint)){
		CollisionPoint *cp;
		cp = dynamic_cast<CollisionPoint*>(kOther);
		cout<<"Point VS Point"<<endl;
		
	} else if(bContinue){
		cout<<"Unhandled Collision,Asking kOther"<<endl;	
		kOther->Collide(this,false);
	
	}
		
	return false;
}






