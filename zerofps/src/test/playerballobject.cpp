#include "playerballobject.h"


PlayerBallObject::PlayerBallObject(HeightMap *pkMap,Input *pkInput,ZeroFps *pkFps) {

//	AddProperty(new ModelProperty());
//	AddProperty(new CollisionProperty(&m_kPos,new float(1)));
	AddProperty(new CollisionProperty(&m_kPos));
	AddProperty(new PlayerControlProperty(pkInput,pkFps,pkMap));
}

/*
void PlayerBallObject::HandleCollision(Object* pkObject,bool bContinue){
//	Remove();


}
*/


