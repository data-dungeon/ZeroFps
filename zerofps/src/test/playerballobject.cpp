#include "playerballobject.h"


PlayerBallObject::PlayerBallObject(HeightMap *pkMap,Input *pkInput) {

	AddProperty(new CollisionProperty(&m_kPos,new float(1)));
	AddProperty(new PlayerControlProperty(pkInput,pkMap));
}


void PlayerBallObject::HandleCollision(Object* pkObject,Vector3 kPos,bool bContinue){
	
	if(typeid(*pkObject)==typeid(HeightMapObject)){
		HeightMapObject *kO=dynamic_cast<HeightMapObject*>(pkObject);

		if(GetProperty("PlayerControlProperty")!=NULL) {
			PlayerControlProperty *pc= dynamic_cast<PlayerControlProperty*>(GetProperty("PlayerControlProperty"));
			pc->Bounce(kPos);
		}
	} else if(bContinue){
		pkObject->HandleCollision(this,kPos,false);
	}

}



