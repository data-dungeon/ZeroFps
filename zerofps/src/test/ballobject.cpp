#include "ballobject.h"


BallObject::BallObject(HeightMap *pkMap) {
//	AddProperty(new ModelProperty());
	AddProperty(new CollisionProperty(&m_kPos,new float(3)));
//	AddProperty(new CollisionProperty(&m_kPos));
}

void BallObject::HandleCollision(Object* pkOther,Vector3 kPos,bool bContinue){
	
	//if a ball colides with the player destroy it
	if(typeid(*pkOther)==typeid(HeightMapObject)){
		HeightMapObject *kO=dynamic_cast<HeightMapObject*>(pkOther);

		if(GetProperty("PlayerControlProperty")!=NULL) {
			PlayerControlProperty *pc= dynamic_cast<PlayerControlProperty*>(GetProperty("PlayerControlProperty"));
			pc->Bounce(kPos);
		}	
	} else if(typeid(*pkOther)==typeid(PlayerBallObject)){
		PlayerBallObject *kO=dynamic_cast<PlayerBallObject*>(pkOther);
		Remove();	
		
	} else if(bContinue){
		pkOther->HandleCollision(this,kPos,false);
	}

}



