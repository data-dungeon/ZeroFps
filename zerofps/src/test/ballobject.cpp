#include "ballobject.h"


BallObject::BallObject(HeightMap *pkMap,ZeroFps *pkFps) {
//	AddProperty(new ModelProperty());
	AddProperty(new CollisionProperty(&m_kPos,new float(1)));
//	AddProperty(new CollisionProperty(&m_kPos));
	AddProperty(new GravityProperty(pkFps));

	m_pkFps=pkFps;
	onGround=false;
}

void BallObject::HandleCollision(Object* pkOther,Vector3 kPos,bool bContinue){
	
	//if a ball colides with the player destroy it
	if(typeid(*pkOther)==typeid(HeightMapObject)){
		HeightMapObject *kO=dynamic_cast<HeightMapObject*>(pkOther);
	
	GetPos()=kPos;
	m_kVel.y=0;
	onGround=true;
		
	} else if(typeid(*pkOther)==typeid(PlayerBallObject)){
		PlayerBallObject *kO=dynamic_cast<PlayerBallObject*>(pkOther);
		Remove();	
		
	} else if(bContinue){
		pkOther->HandleCollision(this,kPos,false);
	}

}



