#include "collisionsphere.h"

CollisionSphere::CollisionSphere() {


}

bool CollisionSphere::Collide(CollisionObject *kOther,bool bContinue) {
//	cout<<typeid(*kOther).name()<<endl;
//	cout<<typeid(CollisionSphere).name()<<endl;	
	if(typeid(*kOther)==typeid(CollisionSphere)){
		cout<<"SPHERE VS SPHERE"<<endl;

	} else if(typeid(*kOther)==typeid(CollisionPoint)){
		cout<<"SPHERE VS POINT"<<endl;
	
	} else if(bContinue){
		cout<<"Unhandled Collision,Asking kOther"<<endl;	
		kOther->Collide(this,false);
	
	}
		
	return false;
}






