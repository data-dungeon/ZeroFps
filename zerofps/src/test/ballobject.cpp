#include "ballobject.h"


BallObject::BallObject(HeightMap *pkMap) {
	AddProperty(new ModelProperty());
	AddProperty(new CollisionProperty(&m_kPos,new float(1)));

}

void BallObject::HandleCollision(Object* pkOther,bool bContinue){
	
	//if a ball colides with the player destroy it
	if(typeid(*pkOther)==typeid(PlayerBallObject)){
		PlayerBallObject *kO=dynamic_cast<PlayerBallObject*>(pkOther);
		Remove();	
		
	} else if(bContinue){
		pkOther->HandleCollision(this,false);
	}

}



